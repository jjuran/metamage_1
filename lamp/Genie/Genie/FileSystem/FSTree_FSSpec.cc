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
#include "Nitrogen/Files.h"

// POSeven
#include "POSeven/Errno.hh"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FileSystem/FSTree_Dev.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Proc.hh"
#include "Genie/FileSystem/FSTree_RsrcFile.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	
	class FSTree_FSSpec : public FSTree_Mappable
	{
		private:
			FSSpec fileSpec;
		
		public:
			FSTree_FSSpec( const FSSpec& file ) : fileSpec( file )  {}
			
			FSTreePtr Self()   const;
			FSTreePtr Parent() const;
			
			void Stat( struct ::stat& sb ) const;
			
			void ChangeMode( mode_t mode ) const;
			
			void Delete() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			boost::shared_ptr< IOHandle > Open( OpenFlags flags              ) const;
			
			void Exec( const char* const argv[], const char* const envp[] ) const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup_Regular( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	struct Volumes_Details
	{
		static FSTreePtr Lookup( const std::string& name );
		
		static N::Volume_Container ItemSequence()  { return N::Volumes(); }
		
		static FSNode ConvertToFSNode( N::FSVolumeRefNum vRefNum );
	};
	
	class FSTree_Volumes : public FSTree_Special< Volumes_Details >
	{
		private:
			typedef FSTree_Special< Volumes_Details > Base;
		
		public:
			void Stat( struct ::stat& sb ) const;
	};
	
	
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
	
	static N::FSDirSpec FindJDirectory()
	{
		FSSpec result;
		
		// Try current directory first
		if ( N::FSpTestDirectoryExists( result = N::FSDirSpec() & "j" ) )
		{
			return NN::Convert< N::FSDirSpec >( result );
		}
		
		// Then root, or bust
		return N::RootDirectory( N::BootVolume() ) << "j";
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
		
		FSTreePtr result( tree = new FSTree_FSSpec( FindJDirectory() & "" ) );
		
		FSTreePtr users( new FSTree_FSSpec( N::RootDirectory( N::BootVolume() ) & "Users" ) );
		
		tree->Map( "Users",   users );
		tree->Map( "Volumes", FSTreePtr( new FSTree_Volumes() ) );
		
		tree->Map( "proc", GetProcFSTree() );
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
		
		std::string name = UnixFromMacName( NN::Convert< std::string >( volume.name ) );
		
		FSTreePtr tree( new FSTree_FSSpec( volume ) );
		
		return FSNode( name, tree );
	}
	
	
	static boost::shared_ptr< RegularFileHandle > OpenFile( NN::Owned< N::FSFileRefNum > refNum )
	{
		return boost::shared_ptr< RegularFileHandle >( new RegularFileHandle( refNum ) );
	}
	
	static boost::shared_ptr< IOHandle > OpenFSSpec( const FSSpec& fileSpec, OpenFlags flags, bool rsrcFork )
	{
		unsigned char rdPerm = flags + 1  &  FREAD;
		unsigned char wrPerm = flags + 1  &  FWRITE;
		
		bool nonblocking = flags & O_NONBLOCK;
		bool appending   = flags & O_APPEND;
		// ...
		bool creating    = flags & O_CREAT;
		bool truncating  = flags & O_TRUNC;
		bool excluding   = flags & O_EXCL;
		// ...
		bool resFork     = flags & O_ResFork;
		bool resMap      = flags & O_ResMap;
		
		NN::Owned< N::FSFileRefNum > fileH = rsrcFork ? N::FSpOpenRF( fileSpec, rdPerm | wrPerm )
		                                              : N::FSpOpenDF( fileSpec, rdPerm | wrPerm );
		
		if ( truncating )
		{
			N::SetEOF( fileH, 0 );
		}
		else if ( appending )
		{
			N::SetFPos( fileH, fsFromLEOF, 0 );
		}
		
		return OpenFile( fileH );
	}
	
	
	FSTreePtr FSTree_FSSpec::Self() const
	{
		return FSTreePtr( new FSTree_FSSpec( *this ) );
	}
	
	FSTreePtr FSTree_FSSpec::Parent() const
	{
		if ( NN::Convert< N::FSDirSpec >( fileSpec ) == FindJDirectory() )
		{
			return FSRoot();
		}
		
		if ( fileSpec.parID == fsRtParID )
		{
			return FSTree_Volumes::GetSingleton();
		}
		
		return FSTreePtr( new FSTree_FSSpec( NN::Convert< FSSpec >( N::FSpGetParent( fileSpec ) ) ) );
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
	
	boost::shared_ptr< IOHandle > FSTree_FSSpec::Open( OpenFlags flags, mode_t mode ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		if ( creating )
		{
			if ( !N::FSpTestFileExists( fileSpec ) )
			{
				// No need to convert name -- for examination only
				std::string name = NN::Convert< std::string >( fileSpec.name );
				
				FileSignature sig = PickFileSignatureForName( name );
				
				N::FSpCreate( fileSpec, sig.creator, sig.type );
			}
			else if ( excluding )
			{
				//throw N::DupFNErr();
				P7::ThrowErrno( EEXIST );
			}
		}
		
		return Open( flags );
	}
	
	boost::shared_ptr< IOHandle > FSTree_FSSpec::Open( OpenFlags flags ) const
	{
		const bool notRsrcFork = false;
		
		return OpenFSSpec( fileSpec, flags, notRsrcFork );
	}
	
	void FSTree_FSSpec::Exec( const char* const argv[], const char* const envp[] ) const
	{
		CurrentProcess().Exec( fileSpec, argv, envp );
	}
	
	void FSTree_FSSpec::CreateDirectory( mode_t mode ) const
	{
		N::FSpDirCreate( fileSpec );
	}
	
	FSTreePtr FSTree_FSSpec::Lookup_Regular( const std::string& name ) const
	{
		if ( N::FSpTestFileExists( fileSpec )  &&  name == "rsrc" )
		{
			return GetRsrcForkFSTree( fileSpec );
		}
		
		N::FSDirSpec dir = NN::Convert< N::FSDirSpec >( fileSpec );
		
		FSSpec item = dir & MacFromUnixName( name );
		
		return FSTreePtr( new FSTree_FSSpec( item ) );
	}
	
	void FSTree_FSSpec::IterateIntoCache( FSTreeCache& cache ) const
	{
		N::FSDirSpec dir( NN::Convert< N::FSDirSpec >( fileSpec ) );
		
		N::FSSpecContents_Container contents = N::FSContents( dir );
		
		std::transform( contents.begin(),
		                contents.end(),
		                //cache.begin() + 2,
		                std::back_inserter( cache ),
		                std::ptr_fun( MakeFSNode_FSSpec ) );
	}
	
	
	FSTreePtr Volumes_Details::Lookup( const std::string& name )
	{
		N::FSDirSpec rootDir( N::RootDirectory( DetermineVRefNum( name + ":" ) ) );
		
		return FSTreePtr( new FSTree_FSSpec( rootDir & "" ) );
	}
	
	FSNode Volumes_Details::ConvertToFSNode( N::FSVolumeRefNum vRefNum )
	{
		FSSpec volume = N::FSMakeFSSpec( vRefNum, N::FSDirID( long( fsRtDirID ) ), "\p" );
		
		std::string name = UnixFromMacName( NN::Convert< std::string >( volume.name ) );
		
		FSTreePtr tree( new FSTree_FSSpec( volume ) );
		
		return FSNode( name, tree );
	}
	
	void FSTree_Volumes::Stat( struct ::stat& sb ) const
	{
		Base::Stat( sb );
		
		sb.st_ino = fsRtParID;
	}
	
}

