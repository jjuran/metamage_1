/*	================
 *	FSTree_FSSpec.cc
 *	================
 */

#include "Genie/FS/FSTree_FSSpec.hh"

// POSIX
#include "fcntl.h"
#include "sys/stat.h"

// Iota
#include "iota/strings.hh"

// MoreFiles
#include "MoreFiles/FileCopy.h"

// mac-config
#include "mac_config/aliases.hh"

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// mac-sys-utils
#include "mac_sys/async_wakeup.hh"
#include "mac_sys/gestalt.hh"
#include "mac_sys/volume_params.hh"

// mac-file-utils
#include "mac_file/make_FSSpec.hh"
#include "mac_file/parent_directory.hh"
#include "mac_file/program_file.hh"

// mac-app-utils
#include "mac_app/create_alias.hh"

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

// MacFeatures
#include "MacFeatures/BlueBoxed.hh"
#include "MacFeatures/Features.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/functions/pathname.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/file_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/misc_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/union.hh"
#include "vfs/primitives/stat.hh"

// MacVFS
#include "MacVFS/util/FSSpec_from_node.hh"

// relix-kernel
#include "relix/api/current_thread.hh"
#include "relix/api/root.hh"
#include "relix/fs/fifo.hh"
#include "relix/task/scheduler.hh"
#include "relix/task/task.hh"
#include "relix/task/thread.hh"

// Genie
#include "Genie/BinaryImage.hh"
#include "Genie/code/prepare_executable.hh"
#include "Genie/config/resfs.hh"
#include "Genie/FileSignature.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_RsrcFile.hh"
#include "Genie/FS/HFS/hashed_long_name.hh"
#include "Genie/FS/HFS/LongName.hh"
#include "Genie/FS/HFS/Rename.hh"
#include "Genie/FS/HFS/SetFileTimes.hh"
#include "Genie/FS/resfs.hh"
#include "Genie/FS/sys/mac/errata.hh"
#include "Genie/FS/sys/mac/vol/list.hh"
#include "Genie/FS/ResFile_Dir.hh"
#include "Genie/FS/Root_Overlay.hh"
#include "Genie/FS/StatFile.hh"
#include "Genie/FS/Users.hh"
#include "Genie/IO/MacDirectory.hh"
#include "Genie/IO/MacFile.hh"
#include "Genie/Kernel/native_syscalls.hh"
#include "Genie/Process/AsyncYield.hh"
#include "Genie/Utilities/AsyncIO.hh"
#include "Genie/Utilities/OpenDataFork.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	using mac::types::VRefNum_DirID;
	using mac::file::make_FSSpec;
	using mac::file::parent_directory;
	
	
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
	
	
	static void finish_creation( const FSSpec& file, const plus::string& name )
	{
		SetLongName( file, slashes_from_colons( plus::mac_from_utf8( name ) ) );
	}
	
	static void create_file( const FSSpec&        file,
	                         const plus::string&  name,
	                         Mac::FSCreator       creator,
	                         Mac::FSType          type )
	{
		N::HCreate( file, creator, type );
		
		finish_creation( file, name );
	}
	
	static void create_file( const FSSpec& file, const plus::string& name )
	{
		N::FileSignature sig = PickFileSignatureForName( name.data(), name.size() );
		
		create_file( file, name, sig.creator, sig.type );
	}
	
	static plus::string SlurpFile( const FSSpec& file )
	{
		plus::string result;
		
		n::owned< N::FSFileRefNum > input = OpenDataFork( file, N::fsRdPerm );
		
		const std::size_t size = N::GetEOF( input );
		
		char* p = result.reset( size );
		
		const std::size_t bytes_read = N::FSRead( input, size, p, N::ThrowEOF_Always() );
		
		return result;
	}
	
	static void SpewFile( const FSSpec& file, const plus::string& contents )
	{
		n::owned< N::FSFileRefNum > output = OpenDataFork( file, N::fsWrPerm );
		
		N::SetEOF( output, 0 );
		
		N::FSWrite( output, contents.size(), contents.data() );
	}
	
	static plus::string get_long_name( const FSSpec& item )
	{
		if ( item.name[0] == 31 )
		{
			try
			{
				char buffer[ 256 ];
				
				long size = N::FSpDTGetComment( item, buffer, sizeof buffer );
				
				if ( size > 31 )
				{
					plus::string comment( buffer, size );
					
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
	
	static inline
	FSVolumeRefNum GetVRefNum( FSVolumeRefNum vRefNum = 0 )
	{
		HParamBlockRec pb;
		
		pb.volumeParam.ioVRefNum  = vRefNum;
		pb.volumeParam.ioNamePtr  = NULL;
		pb.volumeParam.ioVolIndex = 0;  // use ioVRefNum only
		
		Mac::ThrowOSStatus( ::PBHGetVInfoSync( &pb ) );
		
		return pb.volumeParam.ioVRefNum;
	}
	
	
	static const unsigned char* const_root_directory_name = "\p" "/";
	
	static
	VRefNum_DirID FindJDirectory()
	{
		CInfoPBRec cInfo = {{ 0 }};
		
		N::Str63 name = const_root_directory_name;  // overkill
		
		// Try current directory first
		
		Mac::FSVolumeRefNum vRefNum = Mac::FSVolumeRefNum();
		Mac::FSDirID        dirID   = Mac::FSDirID       ();
		
		if ( TARGET_API_MAC_CARBON )
		{
			const ProcessSerialNumber current = { 0, kCurrentProcess };
			
			FSRef location;
			OSErr err = GetProcessBundleLocation( &current, &location );
			
			if ( err == noErr )
			{
				const FSSpec locationSpec = N::FSMakeFSSpec( location );
				
				vRefNum = Mac::FSVolumeRefNum( locationSpec.vRefNum );
				dirID   = Mac::FSDirID       ( locationSpec.parID   );
			}
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
		
		cInfo.dirInfo.ioVRefNum = GetVRefNum( cInfo.dirInfo.ioVRefNum );
		
		return Dir_From_CInfo( cInfo );
	}
	
	const VRefNum_DirID& root_DirSpec()
	{
		static VRefNum_DirID j = FindJDirectory();
		
		return j;
	}
	
	static
	VRefNum_DirID FindUsersDirectory()
	{
		const short vRefNum = mac::file::program_file().vRefNum;
		
		FSSpec users = { vRefNum, fsRtDirID, "\p" "Users" };
		
		return Dir_From_FSSpec( users );
	}
	
	static
	const VRefNum_DirID& GetUsersDirectory()
	{
		static VRefNum_DirID users = FindUsersDirectory();
		
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
		
		FSSpec result;
		
		result.vRefNum = vRefNum;
		result.parID   = parID;
		
		memcpy( result.name, hFileInfo.ioNamePtr, 1 + hFileInfo.ioNamePtr[0] );
		
		return result;
	}
	
	static vfs::node_ptr hfs_parent( const vfs::node* that );
	
	static ino_t hfs_parent_inode( const vfs::node* that )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		return extra.fsspec.parID;
	}
	
	static ino_t hfs_inode( const vfs::node* that );
	
	static void hfs_stat( const vfs::node*  that,
	                      struct stat&      sb );
	
	static void hfs_chmod( const vfs::node*  that,
	                       mode_t            mode );
	
	static void hfs_utime( const vfs::node*       that,
	                       const struct timespec  times[2] );
	
	static void hfs_remove( const vfs::node* that );
	
	static void hfs_rename( const vfs::node*  that,
	                        const vfs::node*  destination );
	
	static vfs::filehandle_ptr hfs_open( const vfs::node* that, int flags, mode_t mode );
	
	static off_t hfs_geteof( const vfs::node* that );
	
	static plus::string hfs_readlink( const vfs::node* that );
	
	static vfs::node_ptr hfs_resolve( const vfs::node* that );
	
	static void hfs_symlink( const vfs::node*     that,
	                         const plus::string&  target );
	
	static vfs::node_ptr hfs_lookup( const vfs::node*     that,
	                                 const plus::string&  name,
	                                 const vfs::node*     parent );
	
	static void hfs_listdir( const vfs::node*    that,
	                         vfs::dir_contents&  cache );
	
	static void hfs_mkdir( const vfs::node*  that,
	                       mode_t            mode );
	
	static vfs::filehandle_ptr hfs_opendir( const vfs::node* that );
	
	static void hfs_copyfile( const vfs::node*  that,
	                          const vfs::node*  dest );
	
	static vfs::program_ptr hfs_loadexec( const vfs::node* that );
	
	static void hfs_mknod( const vfs::node* that, mode_t mode, dev_t dev )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		const mode_t type = mode & S_IFMT;
		
		const Mac::FSCreator Creator = Mac::FSCreator( 'Poof' );
		
		const Mac::FSType Type_FIFO = Mac::FSType( 'FIFO' );
		const Mac::FSType Type_SOCK = Mac::FSType( 'SOCK' );
		
		switch ( type )
		{
			case 0:
			case S_IFREG:
				create_file( extra.fsspec, that->name() );
				break;
			
			case S_IFIFO:
				create_file( extra.fsspec, that->name(), Creator, Type_FIFO );
				break;
			
			case S_IFSOCK:
				create_file( extra.fsspec, that->name(), Creator, Type_SOCK );
				break;
			
			default:
				p7::throw_errno( EPERM );
				break;
		}
	}
	
	static const vfs::item_method_set hfs_item_methods =
	{
		&hfs_stat,
		&hfs_chmod,
		NULL,
		&hfs_utime,
		&hfs_remove,
		&hfs_rename,
	};
	
	static const vfs::data_method_set hfs_data_methods =
	{
		&hfs_open,
		&hfs_geteof,
	};
	
	static const vfs::link_method_set hfs_link_methods =
	{
		&hfs_readlink,
		&hfs_resolve,
		&hfs_symlink
	};
	
	static const vfs::dir_method_set hfs_dir_methods =
	{
		&hfs_lookup,
		&hfs_listdir,
		&hfs_mkdir,
		&hfs_opendir
	};
	
	static const vfs::file_method_set hfs_file_methods =
	{
		NULL,
		&hfs_copyfile,
		NULL,
		&hfs_loadexec,
		&hfs_mknod,
	};
	
	static const vfs::misc_method_set hfs_misc_methods =
	{
		&hfs_parent,
		&hfs_parent_inode,
		&hfs_inode
	};
	
	static const vfs::node_method_set hfs_methods =
	{
		&hfs_item_methods,
		&hfs_data_methods,
		&hfs_link_methods,
		&hfs_dir_methods,
		&hfs_file_methods,
		&hfs_misc_methods
	};
	
	static vfs::node_ptr new_HFS_node( const CInfoPBRec&    cInfo,
	                                   const plus::string&  name,
	                                   const vfs::node*     parent = NULL )
	{
		vfs::node* result = new vfs::node( parent,
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
	
	static void hfs_copyfile( const vfs::node*  that,
	                          const vfs::node*  destination )
	{
		OSErr err;
		
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		const FSSpec& srcFile = extra.fsspec;
		
		const FSSpec destFile = vfs::FSSpec_from_node( *destination );
		
		// Do not resolve links
		
		VRefNum_DirID destDir = parent_directory( destFile );
		
		const bool renaming = memcmp( srcFile.name,
		                              destFile.name,
		                              1 + srcFile.name[0] ) != 0;
		
		ConstStr255Param name = renaming ? destFile.name : NULL;
		
		// FIXME:  This logic should be worked into the file copy routine
		// Maybe use ExchangeFiles() for safety?
		
		err = ::HDelete( destFile.vRefNum, destFile.parID, destFile.name );
		
		if ( err != fnfErr )
		{
			Mac::ThrowOSStatus( err );
		}
		
		FSpFileCopy( srcFile, make_FSSpec( destDir ), name );
	}
	
	
	vfs::node_ptr FSTreeFromFSSpec( const FSSpec& item )
	{
		CInfoPBRec cInfo;
		
		const bool async = false;
		
		FSpGetCatInfo< FNF_Returns >( cInfo,
		                              async,
		                              item );
		
		const plus::string name = MakeName( item );
		
		return new_HFS_node( cInfo, name );
	}
	
	vfs::node_ptr node_from_dirID( short vRefNum, long dirID )
	{
		N::Str31 mac_name = "\p";
		
		CInfoPBRec cInfo;
		
		const bool async = false;
		
		FSpGetCatInfo< FNF_Throws >( cInfo,
		                             async,
		                             vRefNum,
		                             dirID,
		                             mac_name,
		                             0 );
		
		const FSSpec fsspec = FSMakeFSSpec( cInfo );
		
		const plus::string name = MakeName( fsspec );
		
		return new_HFS_node( cInfo, name );
	}
	
	vfs::node_ptr New_FSTree_Users( const vfs::node*     parent,
	                                const plus::string&  name,
	                                const void*          args )
	{
		return FSTreeFromFSDirSpec( GetUsersDirectory() );
	}
	
	
	static vfs::node_ptr MakeFSRoot()
	{
		vfs::node_ptr overlayfs = fixed_dir( NULL,
		                                     plus::string::null,
		                                     Root_Overlay_Mappings );
		
	#if CONFIG_RESFS
		
		vfs::node_ptr resfs = new_resfs_root();
		
	#endif
		
		vfs::node_ptr diskfs = FSTreeFromFSDirSpec( root_DirSpec() );
		
	#if CONFIG_RESFS
		
		vfs::node_ptr bottom = vfs::new_union_directory( NULL,
		                                                 plus::string::null,
		                                                 resfs.get(),
		                                                 diskfs.get() );
		
	#else
		
		const vfs::node_ptr& bottom = diskfs;
		
	#endif
		
		vfs::node_ptr rootfs = vfs::new_union_directory( NULL,
		                                                 plus::string::null,
		                                                 overlayfs.get(),
		                                                 bottom.get() );
		
		return rootfs;
	}
	
	
	static vfs::node_ptr hfs_parent( const vfs::node* that )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		if ( extra.fsspec.parID == fsRtParID )
		{
			return Get_sys_mac_vol_N( N::FSVolumeRefNum( extra.fsspec.vRefNum ) );
		}
		
		const VRefNum_DirID& root = root_DirSpec();
		
		if ( extra.fsspec.vRefNum == root.vRefNum  &&  extra.fsspec.parID == root.dirID )
		{
			return relix::root();
		}
		
		try
		{
			if ( extra.fsspec == make_FSSpec( GetUsersDirectory() ) )
			{
				return relix::root();
			}
		}
		catch ( ... )
		{
		}
		
		return FSTreeFromFSDirSpec( parent_directory( extra.fsspec ) );
	}
	
	static ino_t hfs_inode( const vfs::node* that )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		return extra.cinfo.hFileInfo.ioDirID;
	}
	
	static void hfs_stat( const vfs::node*  that,
	                      struct stat&      sb )
	{
		const bool async = false;
		
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		Stat_HFS( async, &sb, extra.cinfo, extra.fsspec.name, false );
	}
	
	static void hfs_chmod( const vfs::node*  that,
	                       mode_t            mode )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		ChangeFileMode( extra.fsspec, mode );
	}
	
	static void hfs_utime( const vfs::node*       that,
	                       const struct timespec  times[2] )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		SetFileTimes( N::FSVolumeRefNum( extra.fsspec.vRefNum ),
		              N::FSDirID       ( extra.fsspec.parID   ),
		              extra.fsspec.name,
		              times );
	}
	
	static void Delete_HFS( const FSSpec& file )
	{
		// returns fnfErr for directories
		OSErr unlockErr = ::HRstFLock( file.vRefNum, file.parID, file.name );
		OSErr deleteErr = ::HDelete  ( file.vRefNum, file.parID, file.name );
		
		if ( MacFeatures::Is_Running_OSXNative()  &&  unlockErr == noErr  &&  deleteErr == fBsyErr )
		{
			// If we're on OS X and the file was busy, try the native unlink().
			
			const FSRef fsRef = N::FSpMakeFSRef( file );
			
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
	
	static void hfs_remove( const vfs::node* that )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		Delete_HFS( extra.fsspec );
	}
	
	
	static void hfs_rename( const vfs::node*  that,
	                        const vfs::node*  destFile )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		Rename_HFS( extra.fsspec, *destFile );
	}
	
	
	static off_t hfs_geteof( const vfs::node* that )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		Mac::ThrowOSStatus( extra.cinfo.hFileInfo.ioResult );
		
		return extra.cinfo.hFileInfo.ioFlLgLen;
	}
	
	static plus::string hfs_readlink( const vfs::node* that )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		const plus::string target = SlurpFile( extra.fsspec );
		
		if ( !target.empty() )
		{
			return target;
		}
		
		return pathname( *hfs_resolve( that ) );
	}
	
	enum
	{
		gestaltAliasMgrAttr = 'alis',
	};
	
	const bool aliases_present =
		CONFIG_ALIASES  &&
			(CONFIG_ALIASES_GRANTED  ||
				mac::sys::gestalt( gestaltAliasMgrAttr ) );
	
	static vfs::node_ptr hfs_resolve( const vfs::node* that )
	{
		if ( !is_symlink( *that ) )
		{
			return that;
		}
		
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		const HFileInfo& hFileInfo = extra.cinfo.hFileInfo;
		
		const bool exists = hFileInfo.ioResult == noErr;
		
		if ( const bool is_dir = hFileInfo.ioFlAttrib & kioFlAttribDirMask )
		{
			// Symlink to root directory
			return relix::root();
		}
		else
		{
			const FInfo& fInfo = hFileInfo.ioFlFndrInfo;
			
			const plus::string target = SlurpFile( extra.fsspec );
			
			if ( !target.empty() )
			{
				return resolve_pathname( *relix::root(), target, *hfs_parent( that ) );
			}
			else if ( aliases_present  &&  fInfo.fdFlags & kIsAlias )
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
		
		FSSpec parent_spec = make_FSSpec( parent_directory( link_spec ) );
		
		FSRef parent_ref = N::FSpMakeFSRef( parent_spec );
		
		nucleus::mutable_string path = N::FSRefMakePath( parent_ref );
		
		path += '/';
		
		path.append( utf8_link_name.data(), utf8_link_name.size() );
		
		p7::throw_posix_result( native_symlink( target_path,
		                                        path.c_str() ) );
	}
	
	static void CreateSymLink( const vfs::node& linkFile, const plus::string& targetPath )
	{
		FSSpec linkSpec = vfs::FSSpec_from_node( linkFile );
		
		if ( MacFeatures::Is_Running_OSXNative() )
		{
			create_native_symlink( linkSpec, targetPath.c_str() );
			
			return;
		}
		
		if ( ! aliases_present )
		{
			goto no_alias;
		}
		
		try
		{
			VRefNum_DirID linkParent = parent_directory( linkSpec );
			
			// Target path is resolved relative to the location of the link file
			// This throws if a nonterminal path component is missing
			const vfs::node_ptr target = resolve_pathname( *relix::root(), targetPath, *FSTreeFromFSDirSpec( linkParent ) );
			
			// Do not resolve links -- if the target of this link is another symlink, so be it
			
			FSSpec targetSpec = vfs::FSSpec_from_node( *target );
			
			OSErr err = mac::app::create_alias( linkSpec, targetSpec );
			
			if ( err == noErr )
			{
				goto created;
			}
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
		
	no_alias:
		
		// Non-aliases get creator and type for OS X symlinks
		N::HCreate( linkSpec, Mac::kSymLinkCreator, Mac::kSymLinkFileType );
		
	created:
		
		SpewFile( linkSpec, targetPath );
	}
	
	static void hfs_symlink( const vfs::node*     that,
	                         const plus::string&  target )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		CreateSymLink( *that, target );
		
		finish_creation( extra.fsspec, that->name() );
	}
	
	static void fifo_update( vfs::filehandle& fifo, relix::fifo_state state )
	{
		OSErr err;
		
		OSType type = 'FIFa' + state;
		
		const vfs::node& file = *get_file( fifo );
		
		hfs_extra& extra = *(hfs_extra*) file.extra();
		
		// Note:  The update could fail if the file has already been deleted.
		
		FInfo fInfo;
		err = FSpGetFInfo( &extra.fsspec, &fInfo );
		
		if ( err == noErr )
		{
			fInfo.fdType = type;
			
			err = FSpSetFInfo( &extra.fsspec, &fInfo );
		}
	}
	
	static bool hfs_is_fifo( const CInfoPBRec& cInfo )
	{
		if ( cInfo.hFileInfo.ioResult != noErr )
		{
			return false;
		}
		
		const FInfo& fInfo = cInfo.hFileInfo.ioFlFndrInfo;
		
		return fInfo.fdCreator == 'Poof'  &&  (fInfo.fdType & 0xFFFFFF00) == 'FIF\0';
	}
	
	static bool hfs_is_socket( const CInfoPBRec& cInfo )
	{
		if ( cInfo.hFileInfo.ioResult != noErr )
		{
			return false;
		}
		
		const FInfo& fInfo = cInfo.hFileInfo.ioFlFndrInfo;
		
		return fInfo.fdCreator == 'Poof'  &&  fInfo.fdType == 'SOCK';
	}
	
	static vfs::filehandle_ptr hfs_open( const vfs::node* that, int flags, mode_t mode )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		bool created = false;
		
		if ( const bool creating = flags & O_CREAT )
		{
			const bool exists = extra.cinfo.hFileInfo.ioResult == noErr;
			
			if ( !exists )
			{
				create_file( extra.fsspec, that->name() );
				
				created = true;
			}
			else
			{
				const plus::string name_MacRoman = plus::mac_from_utf8( that->name() );
				
				if ( name_MacRoman.size() <= 31 )
				{
					// Long names are case-sensitive due to hashing
					
					const plus::string name = slashes_from_colons( name_MacRoman );
					
					const bool equal = memcmp( name.begin(),
					                           (extra.fsspec.name + 1),
					                           name.size() ) == 0;
					
					if ( !equal )
					{
						N::HRename( extra.fsspec, name );
					}
				}
			}
		}
		
		if ( hfs_is_fifo( extra.cinfo ) )
		{
			return relix::open_fifo( that, flags, &fifo_update );
		}
		
		if ( hfs_is_socket( extra.cinfo ) )
		{
			throw p7::errno_t( ENXIO );
		}
		
		vfs::filehandle_ptr opened = OpenMacFileHandle( extra.fsspec,
		                                                flags,
		                                                &OpenDataFork,
		                                                &FSTreeFromFSSpec );
		
		if ( created )
		{
			hfs_chmod( that, mode );
		}
		
		return opened;
	}
	
	static vfs::program_ptr hfs_loadexec( const vfs::node* that )
	{
	#ifdef __i386__
		
		throw p7::errno_t( ENOEXEC );
		
	#endif
		
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		execution_unit unit = GetBinaryImage( extra.fsspec );
		
		return prepare_executable( unit );
	}
	
	static vfs::filehandle_ptr hfs_opendir( const vfs::node* that )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		const VRefNum_DirID dir = Dir_From_CInfo( extra.cinfo );
		
		return Mac_dir_handle( dir );
	}
	
	static void hfs_mkdir( const vfs::node*  that,
	                       mode_t            mode )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		N::DirCreate( extra.fsspec );
		
		finish_creation( extra.fsspec, that->name() );
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
	
	static
	vfs::node_ptr FSTreePtr_From_Lookup( const VRefNum_DirID&  dir,
	                                     const plus::string&   name,
	                                     const vfs::node*      parent )
	{
		N::Str31 macName = hashed_long_name( slashes_from_colons( plus::mac_from_utf8( name ) ) );
		
		CInfoPBRec cInfo;
		
		const bool async = false;
		
		FSpGetCatInfo< FNF_Returns >( cInfo, async, dir.vRefNum, dir.dirID, macName, 0 );
		
		if ( MacFeatures::Is_BlueBoxed()  &&  is_possibly_masked_symlink( cInfo ) )
		{
			FSSpec spec;
			
			*(VRefNum_DirID*) &spec = dir;
			
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
	
	static vfs::node_ptr hfs_lookup( const vfs::node*     that,
	                                 const plus::string&  name,
	                                 const vfs::node*     parent )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		if ( name == "rsrc"  &&  is_file( *that ) )
		{
			return GetRsrcForkFSTree( extra.fsspec );
		}
		
		if ( name == "r"  &&  is_file( *that ) )
		{
			return Get_ResFileDir_FSTree( that, "r", extra.fsspec );
		}
		
		Mac::ThrowOSStatus( extra.cinfo.dirInfo.ioResult );
		
		VRefNum_DirID dir = Dir_From_CInfo( extra.cinfo );
		
		return FSTreePtr_From_Lookup( dir, name, parent );
	}
	
	
	const UInt16 kMaxItems = 4;
	
	struct NameAndID
	{
		Str31  name;
		long   id;
	};
	
	struct IterateIntoCache_CInfoPBRec
	{
		relix::task*   task;
		CInfoPBRec     cInfo;
		NameAndID      items[ kMaxItems ];
		UInt16         n_items;
		volatile bool  done;
	};
	
	namespace
	{
		
		pascal void IterateIntoCache_Completion( ParamBlockRec* _pb )
		{
			IterateIntoCache_CInfoPBRec& pb = *(IterateIntoCache_CInfoPBRec*) _pb;
			
			CInfoPBRec& cInfo = pb.cInfo;
			
			if ( cInfo.dirInfo.ioResult != noErr )
			{
				goto done;
			}
			
			pb.items[ pb.n_items ].id = cInfo.dirInfo.ioDrDirID;
			
			++pb.n_items;
			
			if ( pb.n_items == kMaxItems )
			{
				goto done;
			}
			
			cInfo.dirInfo.ioNamePtr = pb.items[ pb.n_items ].name;
			
			cInfo.dirInfo.ioNamePtr[ 0 ] = '\0';
			
			cInfo.dirInfo.ioDrDirID = cInfo.dirInfo.ioDrParID;
			
			++cInfo.dirInfo.ioFDirIndex;
			
			(void) ::PBGetCatInfoAsync( &cInfo );
			
			if ( cInfo.dirInfo.ioResult >= 0 )
			{
				// Successfully queued
				return;
			}
			
			// We don't know if this was queued or not, so set done below
			
		done:
			
			pb.done = true;
			
			async_resume_task( pb.task );
			
			mac::sys::request_async_wakeup();
		}
		
	}
	
	static void IterateFilesIntoCache( IterateIntoCache_CInfoPBRec&  pb,
	                                   vfs::dir_contents&            cache )
	{
		CInfoPBRec& cInfo = pb.cInfo;
		
		FSSpec item;
		
		item.vRefNum = cInfo.dirInfo.ioVRefNum;
		item.parID   = cInfo.dirInfo.ioDrDirID;
		item.name[0] = 0;
		
		long dirID = cInfo.dirInfo.ioDrDirID;
		
		const bool async = !TARGET_CPU_68K && mac::sys::item_is_on_server( item ) && !MacFeatures::Is_BlueBoxed();
		
		if ( async )
		{
			cInfo.dirInfo.ioCompletion = N::StaticUPP< N::IOCompletionUPP, IterateIntoCache_Completion >();
		}
		
		UInt16 n_items = 0;
		
		while ( true )
		{
			const UInt16 i = n_items + 1;  // one-based
			
			cInfo.dirInfo.ioNamePtr = pb.items[ 0 ].name;
			cInfo.dirInfo.ioDrDirID = dirID;
			
			cInfo.dirInfo.ioFDirIndex = i;
			
			pb.done = false;
			
			pb.n_items = 0;
			
			pb.items[ 0 ].name[ 0 ] = '\0';
			
			if ( async )
			{
				N::PBGetCatInfoAsync( cInfo, N::FNF_Returns() );
				
				while ( !pb.done )
				{
					AsyncYield();
				}
			}
			else if ( const bool exists = N::PBGetCatInfoSync( cInfo, N::FNF_Returns() ) )
			{
				pb.items[ 0 ].id = cInfo.dirInfo.ioDrDirID;
				
				++pb.n_items;
			}
			
			n_items += pb.n_items;
			
			for ( UInt16 j = 0;  j != pb.n_items;  ++j )
			{
				const ino_t inode = pb.items[ j ].id;  // file or dir ID for inode
				
				N::CopyToPascalString( pb.items[ j ].name + 0, item.name, sizeof item.name - 1 );
				
				const vfs::dir_entry node( inode, GetUnixName( item ) );
				
				cache.push_back( node );
			}
			
			if ( cInfo.dirInfo.ioResult == fnfErr )
			{
				return;
			}
			
			Mac::ThrowOSStatus( cInfo.dirInfo.ioResult );
		}
	}
	
	static void hfs_listdir( const vfs::node*    that,
	                         vfs::dir_contents&  cache )
	{
		hfs_extra& extra = *(hfs_extra*) that->extra();
		
		Mac::ThrowOSStatus( extra.cinfo.hFileInfo.ioResult );
		
		IterateIntoCache_CInfoPBRec pb;
		
		pb.task  = relix::current_thread().get_task();
		pb.cInfo = extra.cinfo;
		
		IterateFilesIntoCache( pb, cache );
	}
	
}

namespace relix
{
	
	const vfs::node* root()
	{
		static vfs::node_ptr root = Genie::MakeFSRoot();
		
		return root.get();
	}
	
}
