/*	================
 *	FSTree_FSSpec.cc
 *	================
 */

#include "Genie/FileSystem/FSTree_FSSpec.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <map>
#include <vector>

// POSIX
#include "fcntl.h"
#include "utime.h"
#include "sys/stat.h"

// MoreFiles
#include "MoreFilesExtras.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/Aliases.h"
#include "Nitrogen/Files.h"

// POSeven
#include "POSeven/Errno.hh"

// TimeOff
#include "TimeOff.hh"

// Arcana / MD5
#include "MD5.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FileSystem/FSSpecForkUser.hh"
#include "Genie/FileSystem/FSTree_Dev.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Proc.hh"
#include "Genie/FileSystem/FSTree_RsrcFile.hh"
#include "Genie/FileSystem/FSTree_sys.hh"
#include "Genie/FileSystem/FSTree_sys_mac_vol.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/MacFile.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	inline bool operator==( const FSSpec& a, const FSSpec& b )
	{
		const std::size_t length = sizeof (SInt16) + sizeof (UInt32) + 1 + a.name[0];
		
		return std::memcmp( &a, &b, length ) == 0;
	}
	
	
	static std::string MacFromUnixName( const std::string& name )
	{
		//ASSERT( name != "."  );
		//ASSERT( name != ".." );
		
		std::string result;
		
		result.resize( name.size() );
		
		std::replace_copy( name.begin(),
		                   name.end(),
		                   result.begin(),
		                   ':',
		                   '/' );
		
		return result;
	}
	
	static std::string UnixFromMacName( const std::string& name )
	{
		std::string result;
		
		result.resize( name.size() );
		
		std::replace_copy( name.begin(),
		                   name.end(),
		                   result.begin(),
		                   '/',
		                   ':' );
		
		return result;
	}
	
	
	static char base32_encode( unsigned x )
	{
		return ( x < 10 ? '0' : 'a' - 10 ) + x;
	}
	
	static FSSpec FSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		std::size_t dot = unixName.find_last_of( "." );
		
		const bool has_dot = dot != unixName.npos;
		
		const unsigned n_delimiters = 1;
		
		const unsigned hash_length = 6;
		
		std::size_t base_length = has_dot ? dot : unixName.size();
		
		std::size_t replaced_length = unixName.size() - 31 + n_delimiters + hash_length;
		
		if ( replaced_length >= base_length )
		{
			p7::throw_errno( ENAMETOOLONG );  // extension is too long
		}
		
		std::size_t shortened_base_length = base_length - replaced_length;
		
		MD5::Result hash = MD5::Digest( unixName.data(), unixName.size() );
		
		std::string macName( unixName.begin(), unixName.begin() + shortened_base_length );
		
		macName += '¥';
		
		for ( int i = 0;  i != hash_length;  ++i )
		{
			macName += base32_encode( hash.data[ i ] >> 3 );
		}
		
		if ( has_dot )
		{
			macName.append( unixName.begin() + dot, unixName.end() );
		}
		
		std::replace( macName.begin(), macName.end(), ':', '/' );
		
		FSSpec result = parent / macName;
		
		return result;
	}
	
	static FSSpec FSSpecForLongUnixName( const N::FSDirSpec&  parent,
	                                     const std::string&   unixName,
	                                     bool                 forCreation )
	{
		FSSpec result = FSSpecForLongUnixName( parent, unixName );
		
		if ( io::item_exists( result ) == forCreation )
		{
			p7::throw_errno( forCreation ? ENAMETOOLONG : ENOENT );
		}
		
		return result;
	}
	
	inline FSSpec NewFSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		return FSSpecForLongUnixName( parent, unixName, true );
	}
	
	inline FSSpec OldFSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		return FSSpecForLongUnixName( parent, unixName, false );
	}
	
	static bool ItemWithLongNameExists( const N::FSDirSpec& parent, const std::string& unixName )
	{
		FSSpec item = FSSpecForLongUnixName( parent, unixName );
		
		return io::item_exists( item );
	}
	
	static void CreateFileWithLongName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		N::FileSignature sig = PickFileSignatureForName( unixName );
		
		FSSpec file = NewFSSpecForLongUnixName( parent, unixName );
		
		N::FSpCreate( file, sig );
		
		N::FSpDTSetComment( file, unixName );
	}
	
	static void CreateDirectoryWithLongName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		FSSpec dir = NewFSSpecForLongUnixName( parent, unixName );
		
		N::FSpDirCreate( dir );
		
		N::FSpDTSetComment( dir, unixName );
	}
	
	
	static std::string GetUnixName( const FSSpec& item )
	{
		if ( item.name[0] == 31 )
		{
			try
			{
				std::string comment = N::FSpDTGetComment( item );
				
				if ( comment.size() > 31 )
				{
					// throws ENOENT if the encoded name doesn't exist
					FSSpec hashed = OldFSSpecForLongUnixName( io::get_preceding_directory( item ), comment );
					
					if ( hashed == item )
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
		
		return UnixFromMacName( io::get_filename_string( item ) );
	}
	
	
	static N::FSVolumeRefNum DetermineVRefNum( ConstStr255Param   name,
	                                           N::FSVolumeRefNum  vRefNum = N::FSVolumeRefNum() )
	{
		::FSVolumeRefNum actualVRefNum;
		N::ThrowOSStatus( ::DetermineVRefNum( name, vRefNum, &actualVRefNum ) );
		
		return N::FSVolumeRefNum( actualVRefNum );
	}
	
	static N::FSVolumeRefNum DetermineVRefNum( const std::string&  name,
	                                           N::FSVolumeRefNum   vRefNum = N::FSVolumeRefNum() )
	{
		return DetermineVRefNum( N::Str27( name ), vRefNum );
	}
	
	
	struct Volume_KeyName_Traits
	{
		typedef N::FSVolumeRefNum Key;
		
		static FSSpec FSSpecFromKey( const Key& key )
		{
			return N::FSMakeFSSpec( key, N::fsRtDirID, "\p" );
		}
		
		static std::string NameFromKey( const Key& key )
		{
			return UnixFromMacName( io::get_filename_string( FSSpecFromKey( key ) ) );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			Key key = DetermineVRefNum( MacFromUnixName( name ) + ":" );
			
			return key;
		}
	};
	
	
	class FSTree_HFS : public FSTree_Mappable
	{
		public:
			FSTree_HFS( const std::string& name = std::string() ) : FSTree_Mappable( FSTreePtr(), name )
			{
				// we override Parent()
			}
			
			bool IsLink() const;
			
			ino_t Inode() const;
			
			void Stat( struct ::stat& sb ) const;
			
			void ChangeMode( mode_t mode ) const;
			
			void SetTimes() const;
			
			void SetTimes( const struct timeval* access,
			               const struct timeval* mod,
			               const struct timeval* backup,
			               const struct timeval* creat ) const;
			
			void Delete() const;
			
			off_t GetEOF() const;
			void  SetEOF( off_t length ) const;
			
			std::string ReadLink() const;
			FSTreePtr ResolveLink() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			boost::shared_ptr< IOHandle > Open( OpenFlags flags              ) const;
			
			MainEntry GetMainEntry() const;
			
			FSTreePtr Lookup_Regular( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
		
		private:
			virtual void CreateFile() const = 0;
	};
	
	class FSTree_FSSpec : public FSTree_HFS
	{
		private:
			FSSpec itsFileSpec;
		
		public:
			FSTree_FSSpec( const FSSpec& file ) : itsFileSpec( file )
			{
			}
			
			FSTree_FSSpec( const N::FSDirSpec& dir ) : itsFileSpec( NN::Convert< FSSpec >( dir ) )
			{
			}
			
			bool Exists() const;
			bool IsFile() const;
			bool IsDirectory() const;
			
			std::string Name() const;
			
			FSTreePtr Parent() const;
			
			FSSpec GetFSSpec( bool forCreation ) const;
			
			ino_t ParentInode() const;
			
			void CreateFile() const;
			
			void CreateDirectory( mode_t mode ) const;
	};
	
	class FSTree_ConflictingName : public FSTree_FSSpec
	{
		public:
			FSTree_ConflictingName( const FSSpec& file ) : FSTree_FSSpec( file )
			{
			}
			
			bool Exists() const;
			bool IsFile() const;
			bool IsDirectory() const;
			
			FSSpec GetFSSpec( bool forCreation ) const;
			
			void CreateFile() const;
			
			void CreateDirectory( mode_t mode ) const;
	};
	
	class FSTree_LongName : public FSTree_HFS
	{
		private:
			N::FSDirSpec  itsParent;
		
		public:
			FSTree_LongName( const N::FSDirSpec&  parent,
			                 const std::string&   name ) : FSTree_HFS ( name   ),
			                                               itsParent  ( parent )
			{
			}
			
			bool Exists() const;
			bool IsFile() const;
			bool IsDirectory() const;
			
			FSTreePtr Parent() const;
			
			FSSpec GetFSSpec( bool forCreation = false ) const;
			
			void CreateFile() const;
			
			void CreateDirectory( mode_t mode ) const;
	};
	
	
	class FSTree_Volumes_Link;
	
	
	struct Volumes_Details : public Volume_KeyName_Traits
	{
		typedef N::Volume_Container Sequence;
		
		typedef FSTree_Volumes_Link ChildNode;
		
		static FSTreePtr Parent()  { return FSRoot(); }
		
		static Sequence ItemSequence()  { return N::Volumes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key )
		{
			return true;  // DetermineVRefNum() only returns valid keys
		}
		
		FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                        const std::string&  name,
		                        const Key&          key ) const;
	};
	
	class FSTree_Volumes : public FSTree_Sequence< Volumes_Details >
	{
		private:
			typedef FSTree_Sequence< Volumes_Details > Base;
		
		public:
			FSTree_Volumes( const FSTreePtr&    parent,
			                const std::string&  name ) : Base( parent, name )
			{
			}
			
			ino_t Inode() const;
	};
	
	
	static const char* const_j_directory_name = "j";
	
	static FSSpec FindJDirectory()
	{
		// Try current directory first
		FSSpec j =  N::FSDirSpec() / const_j_directory_name;
		
		if ( !io::directory_exists( j ) )
		{
			// Then root, or bust
			j = io::system_root< N::FSDirSpec >() / const_j_directory_name;
			
			(void) N::FSDirSpec( j );  // throws if not a dir
		}
		
		return j;
	}
	
	static const FSSpec& GetJDirectory()
	{
		static FSSpec j = FindJDirectory();
		
		return j;
	}
	
	static FSSpec FindUsersDirectory()
	{
		N::FSDirSpec root = io::system_root< N::FSDirSpec >();
		
		FSSpec users = root / "Users";
		
		(void) N::FSDirSpec( users );  // throws if not a dir
		
		return users;
	}
	
	static const FSSpec& GetUsersDirectory()
	{
		static FSSpec users = FindUsersDirectory();
		
		return users;
	}
	
	
	class FSTree_J_Symlink : public FSTree
	{
		public:
			FSTree_J_Symlink( const FSTreePtr& parent ) : FSTree( parent, const_j_directory_name )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const  { return "/"; }
			
			FSTreePtr ResolveLink() const  { return FSRoot(); }
	};
	
	inline bool IsRootDirectory( const FSSpec& fileSpec )
	{
		return fileSpec == GetJDirectory();
	}
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item )
	{
		return FSTreePtr( new FSTree_FSSpec( item ) );
	}
	
	static FSTreePtr FSTreeFromFSSpecRespectingJ( const FSSpec& item )
	{
		if ( IsRootDirectory( item ) )
		{
			FSTreePtr parent( new FSTree_FSSpec( io::get_preceding_directory( item ) ) );
			
			return FSTreePtr( new FSTree_J_Symlink( parent ) );
		}
		
		return FSTreePtr( new FSTree_FSSpec( item ) );
	}
	
	
	static const FSTreePtr& MakeFSRoot()
	{
		FSTree_FSSpec* tree = NULL;
		
		static FSTreePtr result = FSTreePtr( tree = new FSTree_FSSpec( FindJDirectory() ) );
		
		if ( tree != NULL )
		{
			FSTreePtr users( new FSTree_FSSpec( io::system_root< N::FSDirSpec >() / "Users" ) );
			
			tree->Map( users );
			
			tree->Map( FSTreePtr( new FSTree_Volumes( result, "Volumes" ) ) );
			tree->Map( FSTreePtr( new FSTree_proc   ( result, "proc"    ) ) );
			
			tree->Map( Premapped_Factory< dev_Mappings >( result, "dev" ) );
			tree->Map( Premapped_Factory< sys_Mappings >( result, "sys" ) );
		}
		
		return result;
	}
	
	FSTreePtr FSRoot()
	{
		static const FSTreePtr& root = MakeFSRoot();
		
		return root;
	}
	
	
	bool FSTree_FSSpec::Exists() const
	{
		return io::item_exists( itsFileSpec );
	}
	
	bool FSTree_FSSpec::IsFile() const
	{
		return io::file_exists( itsFileSpec );
	}
	
	bool FSTree_FSSpec::IsDirectory() const
	{
		return io::directory_exists( itsFileSpec );
	}
	
	bool FSTree_ConflictingName::Exists() const
	{
		return false;
	}
	
	bool FSTree_ConflictingName::IsFile() const
	{
		return false;
	}
	
	bool FSTree_ConflictingName::IsDirectory() const
	{
		return false;
	}
	
	bool FSTree_LongName::Exists() const
	{
		return ItemWithLongNameExists( itsParent, Name() );
	}
	
	bool FSTree_LongName::IsFile() const
	{
		return Exists() && io::file_exists( GetFSSpec() );
	}
	
	bool FSTree_LongName::IsDirectory() const
	{
		return Exists() && io::directory_exists( GetFSSpec() );
	}
	
	bool FSTree_HFS::IsLink() const
	{
		CInfoPBRec paramBlock;
		
		try
		{
			// GetFSSpec() may throw ENOENT for nonexistent long names
			FSSpec spec = GetFSSpec();
			
			try
			{
				FSpGetCatInfo( spec, paramBlock, Async() );
				
				const HFileInfo& hFileInfo = paramBlock.hFileInfo;
				
				bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
				
				bool isAlias = !isDir  &&  hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias;
				
				return isAlias;
			}
			catch ( const N::FNFErr& err )
			{
			#ifdef __MWERKS__
				
				if ( err != fnfErr )
				{
					throw;
				}
				
			#endif
			}
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != ENOENT )
			{
				throw;
			}
		}
		
		return false;
	}
	
	std::string FSTree_FSSpec::Name() const
	{
		if ( itsFileSpec.parID == fsRtParID )
		{
			return "mnt";
		}
		
		if ( IsRootDirectory( itsFileSpec ) )
		{
			return "";
		}
		
		return GetUnixName( itsFileSpec );
	}
	
	inline FSTreePtr Get_sys_mac_vol_N( N::FSVolumeRefNum vRefNum )
	{
		return sys_mac_vol_Details::GetChildNode( ResolvePathname( "/sys/mac/vol" ),
		                                          VRefNum_KeyName_Traits::NameFromKey( vRefNum ),
		                                          vRefNum );
	}
	
	FSTreePtr FSTree_FSSpec::Parent() const
	{
		if ( itsFileSpec.parID == fsRtParID )
		{
			return Get_sys_mac_vol_N( N::FSVolumeRefNum( itsFileSpec.vRefNum ) );
		}
		
		if ( IsRootDirectory( itsFileSpec ) || itsFileSpec == GetUsersDirectory() )
		{
			return FSRoot();
		}
		
		return FSTreePtr( new FSTree_FSSpec( io::get_preceding_directory( itsFileSpec ) ) );
	}
	
	FSTreePtr FSTree_LongName::Parent() const
	{
		return FSTreePtr( new FSTree_FSSpec( itsParent ) );
	}
	
	FSSpec FSTree_FSSpec::GetFSSpec( bool forCreation ) const
	{
		return itsFileSpec;
	}
	
	ino_t FSTree_FSSpec::ParentInode() const
	{
		return itsFileSpec.parID;
	}
	
	FSSpec FSTree_ConflictingName::GetFSSpec( bool forCreation ) const
	{
		p7::throw_errno( forCreation ? EEXIST : ENOENT );
		
		return FSSpec();
	}
	
	FSSpec FSTree_LongName::GetFSSpec( bool forCreation ) const
	{
		return FSSpecForLongUnixName( itsParent, Name(), forCreation );
	}
	
	ino_t FSTree_HFS::Inode() const
	{
		struct ::stat sb;
		
		Stat( sb );
		
		return sb.st_ino;
	}
	
	void FSTree_HFS::Stat( struct ::stat& sb ) const
	{
		StatFile( GetFSSpec(), &sb, false );
	}
	
	void FSTree_HFS::ChangeMode( mode_t mode ) const
	{
		ChangeFileMode( GetFSSpec(), mode );
	}
	
	void FSTree_HFS::SetTimes() const
	{
		using namespace TimeOff;
		
		UInt32 modTime = N::GetDateTime();
		
		FSSpec filespec = GetFSSpec();
		
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( GetFSSpec(), paramBlock );
		
		paramBlock.hFileInfo.ioNamePtr = filespec.name;
		paramBlock.hFileInfo.ioDirID   = filespec.parID;
		
		paramBlock.hFileInfo.ioFlMdDat = modTime;
		
		N::PBSetCatInfoSync( paramBlock );
	}
	
	void FSTree_HFS::SetTimes( const struct timeval* access,
	                           const struct timeval* mod,
	                           const struct timeval* backup,
	                           const struct timeval* creat ) const
	{
		using namespace TimeOff;
		
		FSSpec filespec = GetFSSpec();
		
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( GetFSSpec(), paramBlock );
		
		paramBlock.hFileInfo.ioNamePtr = filespec.name;
		paramBlock.hFileInfo.ioDirID   = filespec.parID;
		
		if ( creat )
		{
			paramBlock.hFileInfo.ioFlCrDat = creat->tv_sec + MacToUnixTimeDifference();
		}
		
		if ( mod )
		{
			paramBlock.hFileInfo.ioFlMdDat = mod->tv_sec + MacToUnixTimeDifference();
		}
		
		if ( backup )
		{
			paramBlock.hFileInfo.ioFlBkDat = backup->tv_sec + MacToUnixTimeDifference();
		}
		
		N::PBSetCatInfoSync( paramBlock );
	}
	
	void FSTree_HFS::Delete() const
	{
		FSSpec file = GetFSSpec();
		
		// returns fnfErr for directories
		OSErr unlockErr = ::FSpRstFLock( &file );
		
		OSErr deleteErr = ::FSpDelete( &file );
		
		// Unfortunately, fBsyErr can mean different things.
		// Here we assume it means a directory is not empty.
		
		if ( unlockErr == fnfErr  &&  deleteErr == fBsyErr )
		{
			p7::throw_errno( ENOTEMPTY );
		}
		
		N::ThrowOSStatus( deleteErr );
	}
	
	
	off_t FSTree_HFS::GetEOF() const
	{
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( GetFSSpec(), paramBlock );
		
		return paramBlock.hFileInfo.ioFlLgLen;
	}
	
	void FSTree_HFS::SetEOF( off_t length ) const
	{
		N::SetEOF( N::FSpOpenDF( GetFSSpec(), N::fsWrPerm ), length );
	}
	
	std::string FSTree_HFS::ReadLink() const
	{
		if ( !IsLink() )
		{
			p7::throw_errno( EINVAL );
		}
		
		return ResolveLink()->Pathname();
	}
	
	FSTreePtr FSTree_HFS::ResolveLink() const
	{
		if ( !Exists() )
		{
			return shared_from_this();
		}
		
		FSSpec target = N::ResolveAliasFile( GetFSSpec(), false );
		
		return FSTreePtr( new FSTree_FSSpec( target ) );
	}
	
	void FSTree_FSSpec::CreateFile() const
	{
		// No need to convert name -- for examination only
		std::string name = io::get_filename_string( itsFileSpec );
		
		N::FileSignature sig = PickFileSignatureForName( name );
		
		N::FSpCreate( itsFileSpec, sig );
	}
	
	void FSTree_ConflictingName::CreateFile() const
	{
		p7::throw_errno( EEXIST );
	}
	
	void FSTree_LongName::CreateFile() const
	{
		CreateFileWithLongName( itsParent, Name() );
	}
	
	boost::shared_ptr< IOHandle > FSTree_HFS::Open( OpenFlags flags, mode_t mode ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		bool created = false;
		
		if ( creating )
		{
			if ( !Exists() )
			{
				CreateFile();
				
				created = true;
			}
			else if ( excluding )
			{
				p7::throw_errno( EEXIST );
			}
		}
		
		boost::shared_ptr< IOHandle > opened = Open( flags );
		
		if ( created  &&  (mode & 0200) == 0 )
		{
			N::FSpSetFLock( GetFSSpec() );
		}
		
		return opened;
	}
	
	boost::shared_ptr< IOHandle > FSTree_HFS::Open( OpenFlags flags ) const
	{
		return DataForkUser().OpenFileHandle( GetFSSpec(), flags );
	}
	
	MainEntry FSTree_HFS::GetMainEntry() const
	{
		return GetMainEntryFromFile( GetFSSpec() );
	}
	
	void FSTree_FSSpec::CreateDirectory( mode_t /*mode*/ ) const
	{
		N::FSpDirCreate( itsFileSpec );
	}
	
	void FSTree_ConflictingName::CreateDirectory( mode_t /*mode*/ ) const
	{
		p7::throw_errno( EEXIST );
	}
	
	void FSTree_LongName::CreateDirectory( mode_t /*mode*/ ) const
	{
		CreateDirectoryWithLongName( itsParent, Name() );
	}
	
	FSTreePtr FSTree_HFS::Lookup_Regular( const std::string& name ) const
	{
		FSSpec thisFSSpec = GetFSSpec();
		
		if ( name == "rsrc"  &&  IsFile() )
		{
			return GetRsrcForkFSTree( thisFSSpec );
		}
		
		if ( name.size() > 31 )
		{
			return FSTreePtr( new FSTree_LongName( N::FSDirSpec( thisFSSpec ), name ) );
		}
		
		std::string macName = MacFromUnixName( name );
		
		FSSpec item = thisFSSpec / macName;
		
		// The requested name and the returned name may differ in letter case.
		const bool matchedCase = std::equal( macName.begin(),
		                                     macName.end(),
		                                     (const char*) item.name + 1 );
		
		return matchedCase ? FSTreeFromFSSpecRespectingJ( item )
		                   : FSTreePtr( new FSTree_ConflictingName( item ) );
	}
	
	void FSTree_HFS::IterateIntoCache( FSTreeCache& cache ) const
	{
		CInfoPBRec  pb;
		::Str255    name;
		
		N::FSpGetCatInfo( GetFSSpec(), pb );
		
		N::FSDirSpec dir;
		
		dir.vRefNum = N::FSVolumeRefNum( pb.dirInfo.ioVRefNum );
		dir.dirID   = N::FSDirID       ( pb.dirInfo.ioDrDirID );
		
		const UInt16 n_items = pb.dirInfo.ioDrNmFls;
		
		for ( UInt16 i = 1;  i <= n_items;  ++i )
		{
			N::FSpGetCatInfo( dir, i, pb, name );
			
			ino_t inode = pb.hFileInfo.ioDirID;  // file or dir ID for inode
			
			FSNode node( inode, NN::Convert< std::string >( name ) );
			
			cache.push_back( node );
		}
	}
	
	
	class FSTree_Volumes_Link : public FSTree, public Volume_KeyName_Traits
	{
		private:
			Key itsKey;
		
		public:
			FSTree_Volumes_Link( const FSTreePtr&    parent,
			                     const std::string&  name,
			                     const Key&          key ) : FSTree( parent, name ),
			                                                 itsKey( key    )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const  { return FSTreePtr( new FSTree_FSSpec( FSSpecFromKey( itsKey ) ) ); }
	};
	
	
	FSTreePtr Volumes_Details::GetChildNode( const FSTreePtr&    parent,
		                                     const std::string&  name,
		                                     const Key&          key ) const
	{
		return MakeFSTree( new FSTree_Volumes_Link( parent, name, key ) );
	}
	
	ino_t FSTree_Volumes::Inode() const
	{
		return fsRtParID;
	}
	
}

