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

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FileSystem/FSSpecForkUser.hh"
#include "Genie/FileSystem/FSTree_Dev.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Proc.hh"
#include "Genie/FileSystem/FSTree_RsrcFile.hh"
#include "Genie/FileSystem/FSTree_sys.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/MacFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
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
	
	
	static std::string GetUnixName( const FSSpec& item )
	{
		return UnixFromMacName( io::get_filename_string( item ) );
	}
	
	static FSSpec FSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		std::size_t dot = unixName.find_last_of( "." );
		
		const bool has_dot = dot != unixName.npos;
		
		unsigned n_delimiters = 1 + has_dot;
		
		const unsigned hash_length = 2;
		
		std::size_t base_length = has_dot ? dot : unixName.size();
		
		std::size_t replaced_length = unixName.size() - 31 + n_delimiters + hash_length;
		
		if ( replaced_length >= base_length )
		{
			p7::throw_errno( ENAMETOOLONG );
		}
		
		std::size_t shortened_base_length = base_length - replaced_length;
		
		std::string macName( unixName.begin(), unixName.begin() + shortened_base_length );
		
		macName += 'É';
		
		macName += "xx";
		
		if ( has_dot )
		{
			macName += 'É';
			
			macName.append( unixName.begin() + dot, unixName.end() );
		}
		
		std::replace( macName.begin(), macName.end(), ':', '/' );
		
		FSSpec result = parent / macName;
		
		return result;
	}
	
	static FSSpec NewFSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		FSSpec result = FSSpecForLongUnixName( parent, unixName );
		
		if ( io::item_exists( result ) )
		{
			p7::throw_errno( ENAMETOOLONG );
		}
		
		return result;
	}
	
	static FSSpec OldFSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		FSSpec result = FSSpecForLongUnixName( parent, unixName );
		
		if ( !io::item_exists( result ) )
		{
			p7::throw_errno( ENOENT );
			throw N::FNFErr();
		}
		
		return result;
	}
	
	static FSSpec LookupLongName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		return OldFSSpecForLongUnixName( parent, unixName );
	}
	
	static bool ItemWithLongNameExists( const N::FSDirSpec& parent, const std::string& unixName )
	{
		FSSpec item = FSSpecForLongUnixName( parent, unixName );
		
		return io::item_exists( item );
	}
	
	static void CreateFileWithLongName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		N::FileSignature sig = PickFileSignatureForName( unixName );
		
		N::FSpCreate( NewFSSpecForLongUnixName( parent, unixName ), sig );
	}
	
	static void CreateDirectoryWithLongName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		N::FSpDirCreate( NewFSSpecForLongUnixName( parent, unixName ) );
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
	
	
	class FSTree_FSSpec : public FSTree_Mappable
	{
		private:
			FSSpec itsFileSpec;
		
		public:
			FSTree_FSSpec( const FSSpec& file ) : itsFileSpec( file )  {}
			
			FSTree_FSSpec( const N::FSDirSpec& dir ) : itsFileSpec( NN::Convert< FSSpec >( dir ) )  {}
			
			bool Exists() const;
			bool IsFile() const;
			bool IsDirectory() const;
			bool IsLink() const;
			
			std::string Name() const;
			
			FSTreePtr Parent() const;
			
			FSSpec GetFSSpec() const;
			
			//mode_t FileTypeMode() const;
			//mode_t FilePermMode() const;
			
			void Stat( struct ::stat& sb ) const;
			
			void ChangeMode( mode_t mode ) const;
			
			void Delete() const;
			
			std::string ReadLink() const;
			FSTreePtr ResolveLink() const;
			
			void CreateFile() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			boost::shared_ptr< IOHandle > Open( OpenFlags flags              ) const;
			
			MainEntry GetMainEntry() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup_Regular( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	class FSTree_Volumes_Link;
	
	
	struct Volumes_Details : public Volume_KeyName_Traits
	{
		typedef N::Volume_Container Sequence;
		
		typedef FSTree_Volumes_Link ChildNode;
		
		static std::string Name()  { return "Volumes"; }
		
		Sequence ItemSequence() const  { return N::Volumes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key )
		{
			return true;  // DetermineVRefNum() only returns valid keys
		}
	};
	
	class FSTree_Volumes : public FSTree_Sequence< UniqueDetails< Volumes_Details > >
	{
		private:
			typedef FSTree_Sequence< UniqueDetails< Volumes_Details > > Base;
		
		public:
			void Stat( struct ::stat& sb ) const;
	};
	
	
	static N::FSDirSpec FindJDirectory()
	{
		FSSpec result;
		
		// Try current directory first
		if ( io::directory_exists( result = N::FSDirSpec() / "j" ) )
		{
			return N::FSDirSpec( result );
		}
		
		// Then root, or bust
		result = io::system_root< N::FSDirSpec >() / "j";
		
		return N::FSDirSpec( result );
	}
	
	static N::FSDirSpec UsersDirectory()
	{
		N::FSDirSpec root = io::system_root< N::FSDirSpec >();
		
		FSSpec users = root / "Users";
		
		return N::FSDirSpec( users );
	}
	
	
	class FSTree_J_Symlink : public FSTree
	{
		public:
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return NN::Convert< std::string >( NN::Convert< FSSpec >( FindJDirectory() ).name ); }
			
			FSTreePtr Parent() const  { return FSTreePtr( new FSTree_FSSpec( io::get_preceding_directory( FindJDirectory() ) ) ); }
			
			std::string ReadLink() const  { return "/"; }
			
			FSTreePtr ResolveLink() const  { return FSRoot(); }
	};
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item )
	{
		if ( io::directory_exists( item )  &&  N::FSDirSpec( item ) == FindJDirectory() )
		{
			return GetSingleton< FSTree_J_Symlink >();
		}
		
		return FSTreePtr( new FSTree_FSSpec( item ) );
	}
	
	
	static FSTreePtr MakeFSRoot()
	{
		FSTree_FSSpec* tree = NULL;
		
		FSTreePtr result( tree = new FSTree_FSSpec( FindJDirectory() ) );
		
		FSTreePtr users( new FSTree_FSSpec( io::system_root< N::FSDirSpec >() / "Users" ) );
		
		tree->Map( users );
		
		tree->MapSingleton< FSTree_Volumes >();
		
		tree->Map( GetDevFSTree () );
		tree->Map( GetProcFSTree() );
		tree->Map( GetSingleton< FSTree_sys >() );
		
		return result;
	}
	
	FSTreePtr FSRoot()
	{
		static FSTreePtr root = MakeFSRoot();
		
		return root;
	}
	
	
	static FSNode MakeFSNode_FSSpec( const FSSpec& item )
	{
		std::string name = GetUnixName( item );
		
		FSTreePtr tree( new FSTree_FSSpec( item ) );
		
		return FSNode( name, tree );
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
	
	bool FSTree_FSSpec::IsLink() const
	{
		CInfoPBRec paramBlock;
		
		try
		{
			N::FSpGetCatInfo( itsFileSpec, paramBlock );
		}
		catch ( const N::FNFErr& err )
		{
		#ifdef __MWERKS__
			
			if ( err.Get() != fnfErr )
			{
				throw;
			}
			
		#endif
			
			return false;
		}
		
		const HFileInfo& hFileInfo = paramBlock.hFileInfo;
		
		bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		bool isAlias = !isDir  &&  hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias;
		
		return isAlias;
	}
	
	static bool IsRootDirectory( const FSSpec& fileSpec )
	{
		return io::directory_exists( fileSpec )  &&  N::FSDirSpec( fileSpec ) == FindJDirectory();
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
		
		return io::get_filename_string( itsFileSpec );
	}
	
	FSTreePtr FSTree_FSSpec::Parent() const
	{
		if ( itsFileSpec.parID == fsRtParID )
		{
			return Get_sys_mac_vol_N( N::FSVolumeRefNum( itsFileSpec.vRefNum ) );
		}
		
		if ( io::directory_exists( itsFileSpec ) )
		{
			N::FSDirSpec dir( itsFileSpec );
			
			if ( dir == FindJDirectory() )
			{
				return FSRoot();
			}
			
			try
			{
				if ( dir == UsersDirectory() )
				{
					return FSRoot();
				}
			}
			catch ( ... )
			{
			}
		}
		
		return FSTreePtr( new FSTree_FSSpec( io::get_preceding_directory( itsFileSpec ) ) );
	}
	
	FSSpec FSTree_FSSpec::GetFSSpec() const
	{
		return itsFileSpec;
	}
	
	void FSTree_FSSpec::Stat( struct ::stat& sb ) const
	{
		StatFile( itsFileSpec, &sb, false );
	}
	
	void FSTree_FSSpec::ChangeMode( mode_t mode ) const
	{
		ChangeFileMode( itsFileSpec, mode );
	}
	
	void FSTree_FSSpec::Delete() const
	{
		N::FSpDelete( itsFileSpec );
	}
	
	
	std::string FSTree_FSSpec::ReadLink() const
	{
		if ( !IsLink() )
		{
			p7::throw_errno( EINVAL );
		}
		
		return ResolveLink()->Pathname();
	}
	
	FSTreePtr FSTree_FSSpec::ResolveLink() const
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
	
	boost::shared_ptr< IOHandle > FSTree_FSSpec::Open( OpenFlags flags, mode_t /*mode*/ ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		if ( creating )
		{
			if ( !Exists() )
			{
				CreateFile();
			}
			else if ( excluding )
			{
				p7::throw_errno( EEXIST );
			}
		}
		
		return Open( flags );
	}
	
	boost::shared_ptr< IOHandle > FSTree_FSSpec::Open( OpenFlags flags ) const
	{
		FSSpec target = N::ResolveAliasFile( itsFileSpec, true );
		
		return DataForkUser().OpenFileHandle( target, flags );
	}
	
	MainEntry FSTree_FSSpec::GetMainEntry() const
	{
		return GetMainEntryFromFile( itsFileSpec );
	}
	
	void FSTree_FSSpec::CreateDirectory( mode_t /*mode*/ ) const
	{
		N::FSpDirCreate( itsFileSpec );
	}
	
	FSTreePtr FSTree_FSSpec::Lookup_Regular( const std::string& name ) const
	{
		FSSpec target = N::ResolveAliasFile( itsFileSpec, true );
		
		if ( io::file_exists( target )  &&  name == "rsrc" )
		{
			return GetRsrcForkFSTree( target );
		}
		
		FSSpec item = target / MacFromUnixName( name );
		
		return FSTreeFromFSSpec( item );
	}
	
	void FSTree_FSSpec::IterateIntoCache( FSTreeCache& cache ) const
	{
		N::FSDirSpec dir( itsFileSpec );
		
		N::FSSpecContents_Container contents = N::FSContents( dir );
		
		std::transform( contents.begin(),
		                contents.end(),
		                //cache.begin() + 2,
		                std::back_inserter( cache ),
		                std::ptr_fun( MakeFSNode_FSSpec ) );
	}
	
	
	class FSTree_Volumes_Link : public FSTree
	{
		private:
			typedef N::FSVolumeRefNum Key;
			
			Key itsKey;
		
		public:
			FSTree_Volumes_Link( const Key& key ) : itsKey( key )  {}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return io::get_filename_string( N::FSMakeFSSpec( itsKey, N::fsRtDirID, "\p" ) ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_Volumes >(); }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const  { return FSTreePtr( new FSTree_FSSpec( NN::Make< N::FSDirSpec >( itsKey, N::fsRtDirID ) ) ); }
	};
	
	void FSTree_Volumes::Stat( struct ::stat& sb ) const
	{
		Base::Stat( sb );
		
		sb.st_ino = fsRtParID;
	}
	
}

