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
			
			bool IsRootDirectory() const;
			
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
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			boost::shared_ptr< IOHandle > Open( OpenFlags flags              ) const;
			
			MainEntry GetMainEntry() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup_Regular( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	struct Volumes_Details
	{
		typedef N::Volume_Container Sequence;
		
		static std::string Name()  { return "Volumes"; }
		
		FSTreePtr Lookup( const std::string& name ) const;
		
		Sequence ItemSequence() const  { return N::Volumes(); }
		
		static std::string ChildName( const Sequence::value_type& child )
		{
			FSSpec volume = N::FSMakeFSSpec( child, N::fsRtDirID, "\p" );
			
			return UnixFromMacName( io::get_filename_string( volume ) );
		}
		
		static FSTreePtr ChildNode( const Sequence::value_type& child )
		{
			FSSpec volume = N::FSMakeFSSpec( child, N::fsRtDirID, "\p" );
			
			return FSTreePtr( new FSTree_FSSpec( volume ) );
		}
	};
	
	class FSTree_Volumes : public FSTree_Special_Unique< Volumes_Details >
	{
		private:
			typedef FSTree_Special_Unique< Volumes_Details > Base;
		
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
		std::string name = UnixFromMacName( io::get_filename_string( item ) );
		
		FSTreePtr tree( new FSTree_FSSpec( item ) );
		
		return FSNode( name, tree );
	}
	
	
	static boost::shared_ptr< IOHandle > OpenFile( NN::Owned< N::FSFileRefNum > refNum )
	{
		return boost::shared_ptr< IOHandle >( new MacDataForkHandle( refNum ) );
	}
	
	static boost::shared_ptr< IOHandle > OpenFSSpec( const FSSpec& fileSpec, OpenFlags flags, bool rsrcFork )
	{
		N::FSIOPermissions rdPerm = N::FSIOPermissions( flags + 1  &  FREAD  );
		N::FSIOPermissions wrPerm = N::FSIOPermissions( flags + 1  &  FWRITE );
		
		bool nonblocking = flags & O_NONBLOCK;
		bool appending   = flags & O_APPEND;
		// ...
		bool creating    = flags & O_CREAT;
		bool truncating  = flags & O_TRUNC;
		bool excluding   = flags & O_EXCL;
		// ...
		
		#if 0
		bool resFork     = flags & O_ResFork;
		bool resMap      = flags & O_ResMap;
		#endif
		
		NN::Owned< N::FSFileRefNum > fileH = rsrcFork ? N::FSpOpenRF( fileSpec, rdPerm | wrPerm )
		                                              : N::FSpOpenDF( fileSpec, rdPerm | wrPerm );
		
		if ( truncating )
		{
			N::SetEOF( fileH, 0 );
		}
		else if ( appending )
		{
			N::SetFPos( fileH, N::fsFromLEOF, 0 );
		}
		
		return OpenFile( fileH );
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
			
			if ( err != N::FNFErr )
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
	
	bool FSTree_FSSpec::IsRootDirectory() const
	{
		return io::directory_exists( itsFileSpec )  &&  N::FSDirSpec( itsFileSpec ) == FindJDirectory();
	}
	
	std::string FSTree_FSSpec::Name() const
	{
		if ( IsRootDirectory() )
		{
			return "";
		}
		
		if ( itsFileSpec.parID == fsRtParID )
		{
			return "mnt";
		}
		
		return io::get_filename_string( itsFileSpec );
	}
	
	FSTreePtr FSTree_FSSpec::Parent() const
	{
		if ( IsRootDirectory() )
		{
			return FSRoot();
		}
		
		if ( itsFileSpec.parID == fsRtParID )
		{
			return Get_sys_mac_vol_N( N::FSVolumeRefNum( itsFileSpec.vRefNum ) );
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
		FSSpec target = N::ResolveAliasFile( itsFileSpec, true );
		
		return FSTreePtr( new FSTree_FSSpec( target ) );
	}
	
	boost::shared_ptr< IOHandle > FSTree_FSSpec::Open( OpenFlags flags, mode_t /*mode*/ ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		if ( creating )
		{
			if ( !io::file_exists( itsFileSpec ) )
			{
				// No need to convert name -- for examination only
				std::string name = io::get_filename_string( itsFileSpec );
				
				N::FileSignature sig = PickFileSignatureForName( name );
				
				N::FSpCreate( itsFileSpec, sig );
			}
			else if ( excluding )
			{
				//throw N::DupFNErr();
				p7::throw_errno( EEXIST );
			}
		}
		
		return Open( flags );
	}
	
	boost::shared_ptr< IOHandle > FSTree_FSSpec::Open( OpenFlags flags ) const
	{
		const bool notRsrcFork = false;
		
		FSSpec target = N::ResolveAliasFile( itsFileSpec, true );
		
		return OpenFSSpec( target, flags, notRsrcFork );
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
	
	FSTreePtr Volumes_Details::Lookup( const std::string& name ) const
	{
		N::FSVolumeRefNum vRefNum = DetermineVRefNum( MacFromUnixName( name ) + ":" );
		
		return FSTreePtr( new FSTree_Volumes_Link( vRefNum ) );
	}
	
	void FSTree_Volumes::Stat( struct ::stat& sb ) const
	{
		Base::Stat( sb );
		
		sb.st_ino = fsRtParID;
	}
	
}

