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
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


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
			FSSpec fileSpec;
		
		public:
			FSTree_FSSpec( const FSSpec& file ) : fileSpec( file )  {}
			
			FSTree_FSSpec( const N::FSDirSpec& dir ) : fileSpec( NN::Convert< FSSpec >( dir ) )  {}
			
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
			
			void Exec( const char* const argv[], const char* const envp[] ) const;
			
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
	
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item )
	{
		return FSTreePtr( new FSTree_FSSpec( item ) );
	}
	
	
	static FSTreePtr MakeFSRoot()
	{
		FSTree_FSSpec* tree = NULL;
		
		FSTreePtr result( tree = new FSTree_FSSpec( FindJDirectory() ) );
		
		FSTreePtr users( new FSTree_FSSpec( io::system_root< N::FSDirSpec >() / "Users" ) );
		
		tree->Map( "Users",   users );
		
		tree->MapSingleton< FSTree_Volumes >();
		
		tree->Map( "proc", GetProcFSTree() );
		tree->Map( "sys",  GetSingleton< FSTree_sys >() );
		tree->Map( "dev",  GetDevFSTree () );
		
		return result;
	}
	
	FSTreePtr FSRoot()
	{
		static FSTreePtr root = MakeFSRoot();
		
		return root;
	}
	
	
	static FSNode MakeFSNode_FSSpec( const FSSpec& item )
	{
		std::string name = UnixFromMacName( NN::Convert< std::string >( item.name ) );
		
		FSTreePtr tree( new FSTree_FSSpec( item ) );
		
		return FSNode( name, tree );
	}
	
	static FSNode MakeFSNode_Volume( N::FSVolumeRefNum vRefNum )
	{
		FSSpec volume = N::FSMakeFSSpec( vRefNum, N::FSDirID( long( fsRtDirID ) ), "\p" );
		
		std::string name = UnixFromMacName( io::get_filename_string( volume ) );
		
		FSTreePtr tree( new FSTree_FSSpec( volume ) );
		
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
		return io::item_exists( fileSpec );
	}
	
	bool FSTree_FSSpec::IsFile() const
	{
		return io::file_exists( fileSpec );
	}
	
	bool FSTree_FSSpec::IsDirectory() const
	{
		return io::directory_exists( fileSpec );
	}
	
	bool FSTree_FSSpec::IsLink() const
	{
		CInfoPBRec paramBlock;
		
		try
		{
			N::FSpGetCatInfo( fileSpec, paramBlock );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err == fnfErr )
			{
				return false;
			}
			
			throw;
		}
		
		const HFileInfo& hFileInfo = paramBlock.hFileInfo;
		
		bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		bool isAlias = !isDir  &&  hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias;
		
		return isAlias;
	}
	
	bool FSTree_FSSpec::IsRootDirectory() const
	{
		try
		{
			return N::FSDirSpec( fileSpec ) == FindJDirectory();
		}
		catch ( ... )
		{
			// Not a directory
		}
		
		return false;
	}
	
	std::string FSTree_FSSpec::Name() const
	{
		if ( IsRootDirectory() )
		{
			return "";
		}
		
		return io::get_filename_string( fileSpec );
	}
	
	FSTreePtr FSTree_FSSpec::Parent() const
	{
		if ( IsRootDirectory() )
		{
			return FSRoot();
		}
		
		if ( fileSpec.parID == fsRtParID )
		{
			return GetSingleton< FSTree_Volumes >();
		}
		
		return FSTreePtr( new FSTree_FSSpec( io::get_preceding_directory( fileSpec ) ) );
	}
	
	FSSpec FSTree_FSSpec::GetFSSpec() const
	{
		return fileSpec;
	}
	
	void FSTree_FSSpec::Stat( struct ::stat& sb ) const
	{
		StatFile( fileSpec, &sb, false );
	}
	
	void FSTree_FSSpec::ChangeMode( mode_t mode ) const
	{
		ChangeFileMode( fileSpec, mode );
	}
	
	void FSTree_FSSpec::Delete() const
	{
		N::FSpDelete( fileSpec );
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
		FSSpec target = N::ResolveAliasFile( fileSpec, true );
		
		return FSTreePtr( new FSTree_FSSpec( target ) );
	}
	
	boost::shared_ptr< IOHandle > FSTree_FSSpec::Open( OpenFlags flags, mode_t /*mode*/ ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		if ( creating )
		{
			if ( !io::file_exists( fileSpec ) )
			{
				// No need to convert name -- for examination only
				std::string name = io::get_filename_string( fileSpec );
				
				N::FileSignature sig = PickFileSignatureForName( name );
				
				N::FSpCreate( fileSpec, sig );
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
		
		FSSpec target = N::ResolveAliasFile( fileSpec, true );
		
		return OpenFSSpec( target, flags, notRsrcFork );
	}
	
	void FSTree_FSSpec::Exec( const char* const argv[], const char* const envp[] ) const
	{
		CurrentProcess().Exec( fileSpec, argv, envp );
	}
	
	void FSTree_FSSpec::CreateDirectory( mode_t /*mode*/ ) const
	{
		N::FSpDirCreate( fileSpec );
	}
	
	FSTreePtr FSTree_FSSpec::Lookup_Regular( const std::string& name ) const
	{
		FSSpec target = N::ResolveAliasFile( fileSpec, true );
		
		if ( io::file_exists( target )  &&  name == "rsrc" )
		{
			return GetRsrcForkFSTree( target );
		}
		
		FSSpec item = target / MacFromUnixName( name );
		
		return FSTreePtr( new FSTree_FSSpec( item ) );
	}
	
	void FSTree_FSSpec::IterateIntoCache( FSTreeCache& cache ) const
	{
		N::FSDirSpec dir( fileSpec );
		
		N::FSSpecContents_Container contents = N::FSContents( dir );
		
		std::transform( contents.begin(),
		                contents.end(),
		                //cache.begin() + 2,
		                std::back_inserter( cache ),
		                std::ptr_fun( MakeFSNode_FSSpec ) );
	}
	
	
	FSTreePtr Volumes_Details::Lookup( const std::string& name ) const
	{
		N::FSVolumeRefNum vRefNum = DetermineVRefNum( MacFromUnixName( name ) + ":" );
		
		N::FSDirSpec rootDir( NN::Make< N::FSDirSpec >( vRefNum, N::fsRtDirID ) );
		
		return FSTreePtr( new FSTree_FSSpec( rootDir ) );
	}
	
	void FSTree_Volumes::Stat( struct ::stat& sb ) const
	{
		Base::Stat( sb );
		
		sb.st_ino = fsRtParID;
	}
	
}

