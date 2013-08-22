/*	================
 *	FSTree_FSSpec.cc
 *	================
 */

#include "Genie/FS/FSTree_FSSpec.hh"

// Standard C++
#include <algorithm>

// POSIX
#include "fcntl.h"
#include "sys/stat.h"

// Iota
#include "iota/convert_string.hh"
#include "iota/strings.hh"

// MoreFiles
#include "MoreFiles/FileCopy.h"
#include "MoreFiles/MoreFilesExtras.h"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/replaced_string.hh"

// Nitrogen
#include "Nitrogen/Aliases.hh"
#include "Nitrogen/Files.hh"
#include "Nitrogen/Processes.hh"
#include "Nitrogen/Resources.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// MacFeatures
#include "MacFeatures/BlueBoxed.hh"
#include "MacFeatures/Features.hh"

// MacIO
#include "MacIO/FSMakeFSSpec_Sync.hh"
#include "MacIO/GetCatInfo_Sync.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Arcana / MD5
#include "MD5/MD5.hh"

// Pedestal
#include "Pedestal/WakeUp.hh"

// MacLamp
#include "FSSpec_from_stat.h"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/functions/pathname.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/functions/root.hh"
#include "vfs/primitives/stat.hh"

// Genie
#include "Genie/code/executable_file.hh"
#include "Genie/code/prepare_executable.hh"
#include "Genie/FileSignature.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSSpecForkUser.hh"
#include "Genie/FS/FSTree_RsrcFile.hh"
#include "Genie/FS/HFS/hashed_long_name.hh"
#include "Genie/FS/HFS/LongName.hh"
#include "Genie/FS/HFS/Rename.hh"
#include "Genie/FS/HFS/SetFileTimes.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/file_method_set.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/misc_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/resfs.hh"
#include "Genie/FS/sys/mac/errata.hh"
#include "Genie/FS/sys/mac/vol/list.hh"
#include "Genie/FS/ResFile_Dir.hh"
#include "Genie/FS/Root_Overlay.hh"
#include "Genie/FS/StatFile.hh"
#include "Genie/FS/Union.hh"
#include "Genie/FS/Users.hh"
#include "Genie/IO/MacDirectory.hh"
#include "Genie/IO/MacFile.hh"
#include "Genie/Kernel/native_syscalls.hh"
#include "Genie/Utilities/AsyncIO.hh"
#include "Genie/Utilities/CreateAlias.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	namespace path_descent_operators
	{
		
		static inline FSSpec operator/( const N::FSDirSpec& dir, const unsigned char* name )
		{
			return MacIO::FSMakeFSSpec< FNF_Returns >( dir, name );
		}
		
		static inline FSSpec operator/( const FSSpec& dir, const unsigned char* name )
		{
			return Dir_From_FSSpec( dir ) / name;
		}
		
		static inline FSSpec operator/( const N::FSDirSpec& dir, const plus::string& name )
		{
			return dir / N::Str63( name );
		}
		
		static inline FSSpec operator/( const FSSpec& dir, const plus::string& name )
		{
			return dir / N::Str63( name );
		}
		
	}
	
	using namespace path_descent_operators;
	
	
	static inline bool operator==( const FSSpec& a, const FSSpec& b )
	{
		const std::size_t length = sizeof (SInt16) + sizeof (UInt32) + 1 + a.name[0];
		
		return std::memcmp( &a, &b, length ) == 0;
	}
	
	
	static inline plus::string colons_from_slashes( const plus::string& mac_name )
	{
		return plus::replaced_string( mac_name, '/', ':' );
	}
	
	static inline plus::string slashes_from_colons( const plus::string& unix_name )
	{
		return plus::replaced_string( unix_name, ':', '/' );
	}
	
	
	static plus::string SlurpFile( const FSSpec& file )
	{
		plus::string result;
		
		n::owned< N::FSFileRefNum > input = N::FSpOpenDF( file, N::fsRdPerm );
		
		const std::size_t size = N::GetEOF( input );
		
		char* p = result.reset( size );
		
		const std::size_t bytes_read = N::FSRead( input, size, p, N::ThrowEOF_Always() );
		
		return result;
	}
	
	static void SpewFile( const FSSpec& file, const plus::string& contents )
	{
		n::owned< N::FSFileRefNum > output = N::FSpOpenDF( file, N::fsWrPerm );
		
		N::SetEOF( output, 0 );
		
		N::FSWrite( output, contents.size(), contents.data() );
	}
	
	static plus::string get_long_name( const FSSpec& item )
	{
		if ( item.name[0] == 31 )
		{
			try
			{
				plus::string comment = iota::convert_string< plus::string >( N::FSpDTGetComment( item ) );
				
				if ( comment.size() > 31 )
				{
					plus::string hashed = hashed_long_name( comment );
					
					ASSERT( hashed.size() == 31  &&  "Long filenames must hash to 31 chars" );
					
					if ( std::memcmp( hashed.data(), &item.name[1], 31 ) == 0 )
					{
						// Assume it's a Unix name.  FIXME:  Need better heuristics
						return comment;
					}
				}
			}
			catch ( ... )
			{
			}
		}
		
		return plus::string( item.name );
	}
	
	static plus::string GetUnixName( const FSSpec& item )
	{
		return plus::utf8_from_mac( colons_from_slashes( get_long_name( item ) ) );
	}
	
	static N::FSVolumeRefNum GetVRefNum( N::FSVolumeRefNum  vRefNum = N::FSVolumeRefNum() )
	{
		HParamBlockRec pb;
		
		pb.volumeParam.ioVRefNum  = vRefNum;
		pb.volumeParam.ioNamePtr  = NULL;
		pb.volumeParam.ioVolIndex = 0;  // use ioVRefNum only
		
		Mac::ThrowOSStatus( ::PBHGetVInfoSync( &pb ) );
		
		return N::FSVolumeRefNum( pb.volumeParam.ioVRefNum );
	}
	
	
	static const unsigned char* const_root_directory_name = "\p" "/";
	
	static N::FSDirSpec FindJDirectory()
	{
		CInfoPBRec cInfo = { 0 };
		
		N::Str63 name = const_root_directory_name;  // overkill
		
		// Try current directory first
		
		Mac::FSVolumeRefNum vRefNum = Mac::FSVolumeRefNum();
		Mac::FSDirID        dirID   = Mac::FSDirID       ();
		
		if ( TARGET_API_MAC_CARBON )
		{
			const FSRef location = N::GetProcessBundleLocation( N::CurrentProcess() );
			
			const FSSpec locationSpec = N::FSMakeFSSpec( location );
			
			vRefNum = Mac::FSVolumeRefNum( locationSpec.vRefNum );
			dirID   = Mac::FSDirID       ( locationSpec.parID   );
		}
		
		const bool exists = MacIO::GetCatInfo< FNF_Returns >( cInfo,
		                                                      vRefNum,
		                                                      dirID,
		                                                      name );
		
		if ( !exists )
		{
			MacIO::GetCatInfo< FNF_Returns >( cInfo,
			                                  vRefNum,
			                                  dirID );
		}
		
		cInfo.dirInfo.ioVRefNum = GetVRefNum( N::FSVolumeRefNum( cInfo.dirInfo.ioVRefNum ) );
		
		return Dir_From_CInfo( cInfo );
	}
	
	const Mac::FSDirSpec& root_DirSpec()
	{
		static N::FSDirSpec j = FindJDirectory();
		
		return j;
	}
	
	static N::FSDirSpec FindUsersDirectory()
	{
		N::FSDirSpec root = io::system_root< N::FSDirSpec >();
		
		FSSpec users = root / "\p" "Users";
		
		return Dir_From_FSSpec( users );
	}
	
	static const N::FSDirSpec& GetUsersDirectory()
	{
		static N::FSDirSpec users = FindUsersDirectory();
		
		return users;
	}
	
	
	static plus::string MakeName( const FSSpec& fileSpec )
	{
		if ( fileSpec.parID == fsRtParID )
		{
			return "mnt";
		}
		
		return GetUnixName( fileSpec );
	}
	
	
	struct hfs_extra
	{
		FSSpec      fsspec;
		CInfoPBRec  cinfo;
	};
	
	static FSSpec FSMakeFSSpec( const CInfoPBRec& cInfo )
	{
		const HFileInfo& hFileInfo = cInfo.hFileInfo;
		
		const bool exists = hFileInfo.ioResult == noErr;
		
		const FSVolumeRefNum vRefNum = hFileInfo.ioVRefNum;
		
		const UInt32 parID = exists ? hFileInfo.ioFlParID
		                            : hFileInfo.ioDirID;
		
		FSSpec result = { vRefNum, parID };
		
		memcpy( result.name, hFileInfo.ioNamePtr, 1 + hFileInfo.ioNamePtr[0] );
		
		return result;
	}
	
	static FSTreePtr hfs_parent( const FSTree* node );
	
	static ino_t hfs_parent_inode( const FSTree* node )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		return extra.fsspec.parID;
	}
	
	static ino_t hfs_inode( const FSTree* node );
	
	static void hfs_stat( const FSTree*   node,
	                      struct ::stat&  sb );
	
	static void hfs_chmod( const FSTree*  node,
	                       mode_t         mode );
	
	static void hfs_utime( const FSTree*          node,
	                       const struct timespec  times[2] );
	
	static void hfs_remove( const FSTree* node );
	
	static void hfs_rename( const FSTree*  node,
	                        const FSTree*  destination );
	
	static IOPtr hfs_open( const FSTree* node, int flags, mode_t mode );
	
	static off_t hfs_geteof( const FSTree* node );
	
	static void hfs_seteof( const FSTree* node, off_t length );
	
	static plus::string hfs_readlink( const FSTree* node );
	
	static FSTreePtr hfs_resolve( const FSTree* node );
	
	static void hfs_symlink( const FSTree*        node,
	                         const plus::string&  target );
	
	static FSTreePtr hfs_lookup( const FSTree*        node,
	                             const plus::string&  name,
	                             const FSTree*        parent );
	
	static void hfs_listdir( const FSTree*       node,
	                         vfs::dir_contents&  cache );
	
	static void hfs_mkdir( const FSTree*  node,
	                       mode_t         mode );
	
	static IOPtr hfs_opendir( const FSTree* node );
	
	static void hfs_copyfile( const FSTree*  node,
	                          const FSTree*  dest );
	
	static vfs::program_ptr hfs_loadexec( const FSTree* node );
	
	static const data_method_set hfs_data_methods =
	{
		&hfs_open,
		&hfs_geteof,
		&hfs_seteof
	};
	
	static const link_method_set hfs_link_methods =
	{
		&hfs_readlink,
		&hfs_resolve,
		&hfs_symlink
	};
	
	static const dir_method_set hfs_dir_methods =
	{
		&hfs_lookup,
		&hfs_listdir,
		&hfs_mkdir,
		&hfs_opendir
	};
	
	static const file_method_set hfs_file_methods =
	{
		NULL,
		&hfs_copyfile,
		NULL,
		&hfs_loadexec
	};
	
	static const misc_method_set hfs_misc_methods =
	{
		&hfs_parent,
		&hfs_parent_inode,
		&hfs_inode
	};
	
	static const node_method_set hfs_methods =
	{
		&hfs_stat,
		&hfs_chmod,
		NULL,
		&hfs_utime,
		&hfs_remove,
		&hfs_rename,
		&hfs_data_methods,
		&hfs_link_methods,
		&hfs_dir_methods,
		&hfs_file_methods,
		&hfs_misc_methods
	};
	
	static FSTreePtr new_HFS_node( const CInfoPBRec&    cInfo,
	                               const plus::string&  name,
	                               const FSTree*        parent = NULL )
	{
		FSTree* result = new FSTree( parent,
		                             name,
		                             GetItemMode( cInfo.hFileInfo ),
		                             &hfs_methods,
		                             sizeof (hfs_extra) );
		
		// we override Parent()
		
		ASSERT( !name.empty() );
		
		hfs_extra& extra = *(hfs_extra*) result->extra();
		
		extra.fsspec = FSMakeFSSpec( cInfo );
		extra.cinfo  = cInfo;
		
		extra.cinfo.hFileInfo.ioNamePtr = extra.fsspec.name;
		
		return result;
	}
	
	
	static void FSpFileCopy( const FSSpec&         source,
	                         const FSSpec&         destDir,
	                         const unsigned char*  copyName       = NULL,
	                         void*                 copyBufferPtr  = NULL,
	                         long                  copyBufferSize = 0,
	                         bool                  preflight      = true )
	{
		NUCLEUS_REQUIRE_ERRORS( Nitrogen::FileManager );
		
		Mac::ThrowOSStatus( ::FSpFileCopy( &source,
		                                   &destDir,
		                                   copyName,
		                                   copyBufferPtr,
		                                   copyBufferSize,
		                                   preflight ) );
	}
	
	static void hfs_copyfile( const FSTree*  node,
	                          const FSTree*  destination )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		const FSSpec& srcFile = extra.fsspec;
		
		const FSSpec destFile = GetFSSpecFromFSTree( destination );
		
		// Do not resolve links
		
		N::FSDirSpec destDir = io::get_preceding_directory( destFile );
		
		const bool renaming = !std::equal( srcFile.name,
		                                   srcFile.name + 1 + srcFile.name[0],
		                                   destFile.name );
		
		ConstStr255Param name = renaming ? destFile.name : NULL;
		
		// FIXME:  This logic should be worked into the file copy routine
		// Maybe use ExchangeFiles() for safety?
		
		if ( io::file_exists( destFile ) )
		{
			io::delete_file( destFile );
		}
		
		FSpFileCopy( srcFile, destDir / "\p", name );
	}
	
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item )
	{
		CInfoPBRec cInfo;
		
		const bool async = false;
		
		FSpGetCatInfo< FNF_Returns >( cInfo,
		                              async,
		                              item );
		
		const plus::string name = MakeName( item );
		
		return new_HFS_node( cInfo, name );
	}
	
	FSTreePtr FSTreeFromFSDirSpec( const N::FSDirSpec& dir )
	{
		N::Str31 mac_name = "\p";
		
		CInfoPBRec cInfo;
		
		const bool async = false;
		
		FSpGetCatInfo< FNF_Throws >( cInfo,
		                             async,
		                             dir.vRefNum,
		                             dir.dirID,
		                             mac_name,
		                             0 );
		
		const FSSpec fsspec = FSMakeFSSpec( cInfo );
		
		const plus::string name = MakeName( fsspec );
		
		return new_HFS_node( cInfo, name );
	}
	
	FSTreePtr New_FSTree_Users( const FSTree*        parent,
	                            const plus::string&  name,
	                            const void*          args )
	{
		return FSTreeFromFSDirSpec( GetUsersDirectory() );
	}
	
	
	static FSTreePtr MakeFSRoot()
	{
		vfs::node_ptr overlayfs = fixed_dir( NULL,
		                                     plus::string::null,
		                                     Root_Overlay_Mappings );
		
		vfs::node_ptr resfs = new_resfs_root();
		
		vfs::node_ptr diskfs = FSTreeFromFSDirSpec( root_DirSpec() );
		
		vfs::node_ptr bottom = New_FSTree_Union( NULL,
		                                         plus::string::null,
		                                         resfs.get(),
		                                         diskfs.get() );
		
		vfs::node_ptr rootfs = New_FSTree_Union( NULL,
		                                         plus::string::null,
		                                         overlayfs.get(),
		                                         bottom.get() );
		
		return rootfs;
	}
	
	
	static FSTreePtr hfs_parent( const FSTree* node )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		if ( extra.fsspec.parID == fsRtParID )
		{
			return Get_sys_mac_vol_N( N::FSVolumeRefNum( extra.fsspec.vRefNum ) );
		}
		
		const Mac::FSDirSpec& root = root_DirSpec();
		
		if ( extra.fsspec.vRefNum == root.vRefNum  &&  extra.fsspec.parID == root.dirID )
		{
			return vfs::root();
		}
		
		try
		{
			if ( extra.fsspec == GetUsersDirectory() / "\p" )
			{
				return vfs::root();
			}
		}
		catch ( ... )
		{
		}
		
		return FSTreeFromFSDirSpec( io::get_preceding_directory( extra.fsspec ) );
	}
	
	static ino_t hfs_inode( const FSTree* node )
	{
		struct ::stat sb;
		
		hfs_stat( node, sb );
		
		return sb.st_ino;
	}
	
	static void hfs_stat( const FSTree*   node,
	                      struct ::stat&  sb )
	{
		const bool async = false;
		
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		Stat_HFS( async, &sb, extra.cinfo, extra.fsspec.name, false );
	}
	
	static void hfs_chmod( const FSTree*  node,
	                       mode_t         mode )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		ChangeFileMode( extra.fsspec, mode );
	}
	
	static void hfs_utime( const FSTree*          node,
	                       const struct timespec  times[2] )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		SetFileTimes( N::FSVolumeRefNum( extra.fsspec.vRefNum ),
		              N::FSDirID       ( extra.fsspec.parID   ),
		              extra.fsspec.name,
		              times );
	}
	
	static void Delete_HFS( const FSSpec& fileSpec )
	{
		// returns fnfErr for directories
		OSErr unlockErr = ::FSpRstFLock( &fileSpec );
		
		OSErr deleteErr = ::FSpDelete( &fileSpec );
		
		if ( MacFeatures::Is_Running_OSXNative()  &&  unlockErr == noErr  &&  deleteErr == fBsyErr )
		{
			// If we're on OS X and the file was busy, try the native unlink().
			
			const FSRef fsRef = N::FSpMakeFSRef( fileSpec );
			
			const nucleus::string path = N::FSRefMakePath( fsRef );
			
			p7::throw_posix_result( native_unlink( path.c_str() ) );
			
			return;
		}
		
		// Unfortunately, fBsyErr can mean different things.
		// Here we assume it means a directory is not empty.
		
		if ( unlockErr == fnfErr  &&  deleteErr == fBsyErr )
		{
			p7::throw_errno( ENOTEMPTY );
		}
		
		Mac::ThrowOSStatus( deleteErr );
	}
	
	static void hfs_remove( const FSTree* node )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		Delete_HFS( extra.fsspec );
	}
	
	
	FSSpec GetFSSpecFromFSTree( const FSTreePtr& file )
	{
		struct ::stat stat_buffer = { 0 };
		
		try
		{
			stat( *file, stat_buffer );
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != ENOENT )
			{
				throw;
			}
		}
		
		FSSpec spec;
		
		p7::throw_posix_result( FSSpec_from_stat( stat_buffer, spec ) );
		
		return spec;
	}
	
	static void hfs_rename( const FSTree*  node,
	                        const FSTree*  destFile )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		Rename_HFS( extra.fsspec, *destFile );
	}
	
	
	static off_t hfs_geteof( const FSTree* node )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		Mac::ThrowOSStatus( extra.cinfo.hFileInfo.ioResult );
		
		return extra.cinfo.hFileInfo.ioFlLgLen;
	}
	
	static void hfs_seteof( const FSTree* node, off_t length )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		N::SetEOF( N::FSpOpenDF( extra.fsspec, N::fsWrPerm ), length );
	}
	
	static plus::string hfs_readlink( const FSTree* node )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		const plus::string target = SlurpFile( extra.fsspec );
		
		if ( !target.empty() )
		{
			return target;
		}
		
		return pathname( *hfs_resolve( node ) );
	}
	
	static FSTreePtr hfs_resolve( const FSTree* node )
	{
		if ( !is_symlink( *node ) )
		{
			return node;
		}
		
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		const HFileInfo& hFileInfo = extra.cinfo.hFileInfo;
		
		const bool exists = hFileInfo.ioResult == noErr;
		
		if ( const bool is_dir = hFileInfo.ioFlAttrib & kioFlAttribDirMask )
		{
			// Symlink to root directory
			return vfs::root();
		}
		else
		{
			const FInfo& fInfo = hFileInfo.ioFlFndrInfo;
			
			const bool is_alias = fInfo.fdFlags & kIsAlias;
			
			const plus::string target = SlurpFile( extra.fsspec );
			
			if ( !target.empty() )
			{
				return resolve_pathname( target, *hfs_parent( node ) );
			}
			else if ( is_alias )
			{
				FSSpec target = N::ResolveAliasFile( extra.fsspec, false );
				
				return FSTreeFromFSSpec( target );
			}
			else
			{
				// empty 'slnk' file
			//	throw p7::errno_t( EIO );
			}
		}
		
		// Appease Metrowerks C++, which thinks we don't return a value
		throw p7::errno_t( EIO );
	}
	
	static void create_native_symlink( const FSSpec& link_spec, const char* target_path )
	{
		plus::string utf8_link_name = plus::utf8_from_mac( link_spec.name );
		
		FSSpec parent_spec = N::FSMakeFSSpec( io::get_preceding_directory( link_spec ) );
		
		FSRef parent_ref = N::FSpMakeFSRef( parent_spec );
		
		nucleus::mutable_string path = N::FSRefMakePath( parent_ref );
		
		path += '/';
		
		path.append( utf8_link_name.data(), utf8_link_name.size() );
		
		p7::throw_posix_result( native_symlink( target_path,
		                                        path.c_str() ) );
	}
	
	static void CreateSymLink( const FSTreePtr& linkFile, const plus::string& targetPath )
	{
		FSSpec linkSpec = GetFSSpecFromFSTree( linkFile );
		
		if ( MacFeatures::Is_Running_OSXNative() )
		{
			create_native_symlink( linkSpec, targetPath.c_str() );
			
			return;
		}
		
		N::FSDirSpec linkParent = io::get_preceding_directory( linkSpec );
		
		try
		{
			// Target path is resolved relative to the location of the link file
			// This throws if a nonterminal path component is missing
			const vfs::node_ptr target = resolve_pathname( targetPath, *FSTreeFromFSDirSpec( linkParent ) );
			
			// Do not resolve links -- if the target of this link is another symlink, so be it
			
			FSSpec targetSpec = GetFSSpecFromFSTree( target );
			
			CreateAlias( linkSpec, targetSpec );
			
			goto created;
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != EXDEV )
			{
				throw;
			}
		}
		catch ( const Mac::OSStatus& err )
		{
		}
		
		// Non-aliases get creator and type for OS X symlinks
		N::FSpCreate( linkSpec, Mac::kSymLinkCreator, Mac::kSymLinkFileType );
		
	created:
		
		SpewFile( linkSpec, targetPath );
	}
	
	static void finish_creation( const FSSpec& file, const plus::string& name )
	{
		SetLongName( file, slashes_from_colons( plus::mac_from_utf8( name ) ) );
	}
	
	static void hfs_symlink( const FSTree*        node,
	                         const plus::string&  target )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		CreateSymLink( node, target );
		
		finish_creation( extra.fsspec, node->name() );
	}
	
	static void create_file( const FSSpec& file, const plus::string& name )
	{
		N::FileSignature sig = PickFileSignatureForName( name.data(), name.size() );
		
		N::FSpCreate( file, sig );
		
		finish_creation( file, name );
	}
	
	static IOPtr hfs_open( const FSTree* node, int flags, mode_t mode )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		bool created = false;
		
		if ( const bool creating = flags & O_CREAT )
		{
			const bool exists = extra.cinfo.hFileInfo.ioResult == noErr;
			
			if ( !exists )
			{
				create_file( extra.fsspec, node->name() );
				
				created = true;
			}
			else
			{
				const plus::string name_MacRoman = plus::mac_from_utf8( node->name() );
				
				if ( name_MacRoman.size() <= 31 )
				{
					// Long names are case-sensitive due to hashing
					
					const plus::string name = slashes_from_colons( name_MacRoman );
					
					const bool equal = std::equal( name.begin(),
					                               name.end(),
					                               (const char*) (extra.fsspec.name + 1) );
					
					if ( !equal )
					{
						N::FSpRename( extra.fsspec, name );
					}
				}
			}
		}
		
		const bool async = false;
		
		IOPtr opened = OpenMacFileHandle( extra.fsspec,
		                                  flags,
		                                  async ? &Genie::FSpOpenDF : N::FSpOpenDF,
		                                  &New_DataForkHandle );
		
		if ( created )
		{
			hfs_chmod( node, mode );
		}
		
		return opened;
	}
	
	static vfs::program_ptr hfs_loadexec( const FSTree* node )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		execution_unit unit = load_executable_file( extra.fsspec );
		
		return prepare_executable( unit );
	}
	
	static IOPtr hfs_opendir( const FSTree* node )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		const N::FSDirSpec dir = Dir_From_CInfo( extra.cinfo );
		
		return new MacDirHandle( dir );
	}
	
	static void hfs_mkdir( const FSTree*  node,
	                       mode_t         mode )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		N::FSpDirCreate( extra.fsspec );
		
		finish_creation( extra.fsspec, node->name() );
	}
	
	
	static bool is_possibly_masked_symlink( const CInfoPBRec& cInfo )
	{
		const HFileInfo& hFileInfo = cInfo.hFileInfo;
		
		if ( hFileInfo.ioResult != 0 )
		{
			return false;  // doesn't exist
		}
		
		if ( hFileInfo.ioFlAttrib & kioFlAttribDirMask )
		{
			return false;  // directory
		}
		
		const FInfo& fInfo = hFileInfo.ioFlFndrInfo;
		
		// 'slnk'/'rhap' files show up as 'TEXT'/'MACS' in Classic
		
		return fInfo.fdType == 'TEXT'  &&  fInfo.fdCreator == 'MACS';
	}
	
	static FSTreePtr FSTreePtr_From_Lookup( const N::FSDirSpec&  dir,
	                                        const plus::string&  name,
	                                        const FSTree*        parent )
	{
		N::Str31 macName = hashed_long_name( slashes_from_colons( plus::mac_from_utf8( name ) ) );
		
		CInfoPBRec cInfo;
		
		const bool async = false;
		
		FSpGetCatInfo< FNF_Returns >( cInfo, async, dir.vRefNum, dir.dirID, macName, 0 );
		
		if ( MacFeatures::Is_BlueBoxed()  &&  is_possibly_masked_symlink( cInfo ) )
		{
			FSSpec spec = { dir.vRefNum, dir.dirID };
			
			memcpy( spec.name, macName, 1 + macName[0] );
			
			FSRef ref = N::FSpMakeFSRef( spec );
			
			FSCatalogInfo info;
			
			N::FSGetCatalogInfo( ref, kFSCatInfoFinderInfo, &info, NULL, NULL, NULL );
			
			const FileInfo& fileInfo = *(const FileInfo*) info.finderInfo;
			
			cInfo.hFileInfo.ioFlFndrInfo.fdType    = fileInfo.fileType;
			cInfo.hFileInfo.ioFlFndrInfo.fdCreator = fileInfo.fileCreator;
		}
		
		return new_HFS_node( cInfo, name, parent );
	}
	
	static FSTreePtr hfs_lookup( const FSTree*        node,
	                             const plus::string&  name,
	                             const FSTree*        parent )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		if ( name == "rsrc"  &&  is_file( *node ) )
		{
			return GetRsrcForkFSTree( extra.fsspec );
		}
		
		if ( name == "r"  &&  is_file( *node ) )
		{
			return Get_ResFileDir_FSTree( node, "r", extra.fsspec );
		}
		
		Mac::ThrowOSStatus( extra.cinfo.dirInfo.ioResult );
		
		N::FSDirSpec dir = Dir_From_CInfo( extra.cinfo );
		
		return FSTreePtr_From_Lookup( dir, name, parent );
	}
	
	
	const UInt16 kMaxItems = 4;
	
	struct NameAndID
	{
		N::Str31    name;
		N::FSDirID  id;
	};
	
#ifdef __MACOS__
	
	struct IterateIntoCache_CInfoPBRec : CInfoPBRec
	{
		NameAndID      items[ kMaxItems ];
		UInt16         n_items;
		volatile bool  done;
	};
	
	namespace
	{
		
		pascal void IterateIntoCache_Completion( ParamBlockRec* pb )
		{
			IterateIntoCache_CInfoPBRec& cInfo = *(IterateIntoCache_CInfoPBRec*) pb;
			
			if ( cInfo.dirInfo.ioResult != noErr )
			{
				goto done;
			}
			
			cInfo.items[ cInfo.n_items ].id = N::FSDirID( cInfo.dirInfo.ioDrDirID );
			
			++cInfo.n_items;
			
			if ( cInfo.n_items == kMaxItems )
			{
				goto done;
			}
			
			cInfo.dirInfo.ioNamePtr = cInfo.items[ cInfo.n_items ].name;
			
			cInfo.dirInfo.ioNamePtr[ 0 ] = '\0';
			
			cInfo.dirInfo.ioDrDirID = cInfo.dirInfo.ioDrParID;
			
			++cInfo.dirInfo.ioFDirIndex;
			
			OSErr err = ::PBGetCatInfoAsync( &cInfo );
			
			if ( cInfo.dirInfo.ioResult >= 0 )
			{
				// Successfully queued
				return;
			}
			
			// We don't know if this was queued or not, so set done below
			
		done:
			
			cInfo.done = true;
			
			Ped::WakeUp();
		}
		
	}
	
	static void IterateFilesIntoCache( IterateIntoCache_CInfoPBRec&  pb,
	                                   vfs::dir_contents&            cache )
	{
		FSSpec item = { pb.dirInfo.ioVRefNum, pb.dirInfo.ioDrDirID };
		
		N::FSDirID dirID = N::FSDirID( pb.dirInfo.ioDrDirID );
		
		const bool async = !TARGET_CPU_68K && FileIsOnServer( item ) && !MacFeatures::Is_BlueBoxed();
		
		if ( async )
		{
			pb.dirInfo.ioCompletion = N::StaticUPP< N::IOCompletionUPP, IterateIntoCache_Completion >();
		}
		
		UInt16 n_items = 0;
		
		while ( true )
		{
			const UInt16 i = n_items + 1;  // one-based
			
			pb.dirInfo.ioNamePtr = pb.items[ 0 ].name;
			pb.dirInfo.ioDrDirID = dirID;
			
			pb.dirInfo.ioFDirIndex = i;
			
			pb.done = false;
			
			pb.n_items = 0;
			
			pb.items[ 0 ].name[ 0 ] = '\0';
			
			if ( async )
			{
				N::PBGetCatInfoAsync( pb, N::FNF_Returns() );
				
				while ( !pb.done )
				{
					AsyncYield();
				}
			}
			else if ( const bool exists = N::PBGetCatInfoSync( pb, N::FNF_Returns() ) )
			{
				pb.items[ 0 ].id = N::FSDirID( pb.dirInfo.ioDrDirID );
				
				++pb.n_items;
			}
			
			n_items += pb.n_items;
			
			for ( UInt16 j = 0;  j != pb.n_items;  ++j )
			{
				const ino_t inode = pb.items[ j ].id;  // file or dir ID for inode
				
				N::CopyToPascalString( pb.items[ j ].name, item.name, sizeof item.name - 1 );
				
				const vfs::dir_entry node( inode, GetUnixName( item ) );
				
				cache.push_back( node );
			}
			
			if ( pb.dirInfo.ioResult == fnfErr )
			{
				return;
			}
			
			Mac::ThrowOSStatus( pb.dirInfo.ioResult );
		}
	}
	
#endif
	
	static void hfs_listdir( const FSTree*       node,
	                         vfs::dir_contents&  cache )
	{
		hfs_extra& extra = *(hfs_extra*) node->extra();
		
		Mac::ThrowOSStatus( extra.cinfo.hFileInfo.ioResult );
		
	#ifdef __MACOS__
		
		IterateIntoCache_CInfoPBRec cInfo;
		
		static_cast< CInfoPBRec& >( cInfo ) = extra.cinfo;
		
		IterateFilesIntoCache( cInfo, cache );
		
	#endif
	}
	
}

namespace vfs
{
	
	const node* root()
	{
		static node_ptr root = Genie::MakeFSRoot();
		
		return root.get();
	}
	
}

