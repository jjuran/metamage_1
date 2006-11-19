/*	=========
 *	FSTree.cc
 *	=========
 */

#include "Genie/FileSystem/FSTree.hh"

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
#include "Genie/Devices.hh"
#include "Genie/FileSignature.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	
	static void StatDirectory( struct ::stat& sb )
	{
		mode_t mode = S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR;
		
		sb.st_dev = 0;
		sb.st_ino = 0;
		sb.st_mode = mode;
		sb.st_nlink = 1;
		sb.st_uid = 0;
		sb.st_gid = 0;
		sb.st_rdev = 0;
		sb.st_size = 0;
		sb.st_blksize = 4096;
		sb.st_blocks = 0;
		sb.st_atime = 0;
		sb.st_mtime = 0;
		sb.st_ctime = 0;
		
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
	
	
	static FSTreePtr FSNull();
	
	
	typedef std::vector< FSNode > FSTreeCache;
	
	typedef boost::shared_ptr< const FSTreeCache > FSTreeCachePtr;
	
	class FSIterator_Cache : public FSIterator
	{
		private:
			FSTreeCachePtr  contents;
			std::size_t     nextOffset;
		
		public:
			FSIterator_Cache( const FSTreeCachePtr& cache ) : contents( cache ), nextOffset( 0 )  {}
			
			FSNode Get() const  { return nextOffset < contents->size() ? contents->at( nextOffset ) : FSNode(); }
			
			void Advance()  { ++nextOffset; }
			
			void Rewind()  { nextOffset = 0; }
			
			void Seek( off_t index )  { nextOffset = index; }
			
			off_t Tell() const  { return nextOffset; }
	};
	
	
	template < class FSTree_Type >
	FSTreePtr GetSingleton()
	{
		static FSTreePtr singleton = FSTreePtr( new FSTree_Type() );
		
		return singleton;
	}
	
	
	class FSTree_Null : public FSTree
	{
		public:
			void Stat( struct ::stat& sb ) const  { P7::ThrowErrno( ENOENT ); }
	};
	
	class FSTree_Directory : public FSTree
	{
		public:
			virtual ~FSTree_Directory()  {}
			
			virtual FSTreePtr Self()   const = 0;
			virtual FSTreePtr Parent() const = 0;
			
			void Stat( struct ::stat& sb ) const  { StatDirectory( sb ); }
			
			FSTreePtr Lookup( const std::string& name ) const;
			
			virtual FSTreePtr Lookup_Child( const std::string& name ) const = 0;
			
			FSIteratorPtr Iterate() const;
			
			virtual void IterateIntoCache( FSTreeCache& cache ) const = 0;
	};
	
	template < class Details >
	class FSTree_Special : public FSTree_Directory
	{
		public:
			static FSTreePtr GetSingleton()  { return Genie::GetSingleton< FSTree_Special >(); }
			
			FSTreePtr Self()   const  { return GetSingleton(); }
			FSTreePtr Parent() const  { return FSRoot(); }
			
			FSTreePtr Lookup_Child( const std::string& name ) const  { return Details::Lookup( name ); }
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	class FSTree_Mappable : public FSTree_Directory
	{
		protected:
			typedef std::map< std::string, FSTreePtr > Mappings;
			
			Mappings mappings;
		
		public:
			virtual ~FSTree_Mappable()  {}
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void Map( const std::string& name, FSTreePtr tree );
			
			FSTreePtr Lookup_Mapping( const std::string& name ) const;
			
			virtual FSTreePtr Lookup_Regular( const std::string& name ) const = 0;
	};
	
	class FSTree_RsrcFile : public FSTree
	{
		private:
			FSSpec fileSpec;
		
		public:
			FSTree_RsrcFile( const FSSpec& file ) : fileSpec( file )  {}
			
			void Stat( struct ::stat& sb ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
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
	
	class FSTree_Virtual : public FSTree_Mappable
	{
		public:
			FSTreePtr Lookup_Regular( const std::string& name ) const  { return FSNull(); }
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	class FSTree_dev : public FSTree_Virtual
	{
		public:
			FSTree_dev();
			
			static FSTreePtr GetSingleton()  { return Genie::GetSingleton< FSTree_dev >(); }
			
			FSTreePtr Self()   const  { return GetSingleton(); }
			FSTreePtr Parent() const  { return FSRoot(); }
	};
	
	class FSTree_Device : public FSTree
	{
		private:
			std::string deviceName;
		
		public:
			FSTree_Device( const std::string& name ) : deviceName( name )  {}
			
			void Stat( struct ::stat& sb ) const  { StatGeneric( &sb ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_pid : public FSTree_Directory
	{
		public:
			FSTree_pid( pid_t pid )  {}
			
			FSTreePtr Self()   const;
			FSTreePtr Parent() const;
			
			void Stat( struct ::stat& sb ) const;
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	FSIteratorPtr FSTree_Directory::Iterate() const
	{
		FSTreeCache cache( 2 );
		
		cache[0] = FSNode( ".",  Self()   );
		cache[1] = FSNode( "..", Parent() );
		
		IterateIntoCache( cache );
		
		FSTreeCache* newCache = new FSTreeCache();
		
		FSTreeCachePtr cachePtr( newCache );
		
		std::swap( cache, *newCache );
		
		return FSIteratorPtr( new FSIterator_Cache( cachePtr ) );
	}
	
	
	template < class Details >
	void FSTree_Special< Details >::IterateIntoCache( FSTreeCache& cache ) const
	{
		std::transform( Details::ItemSequence().begin(),
		                Details::ItemSequence().end(),
		                std::back_inserter( cache ),
		                std::ptr_fun( Details::ConvertToFSNode ) );
	}
	
	
	struct Volumes_Details
	{
		static FSTreePtr Lookup( const std::string& name )
		{
			N::FSDirSpec rootDir( N::RootDirectory( DetermineVRefNum( name + ":" ) ) );
			
			return FSTreePtr( new FSTree_FSSpec( rootDir & "" ) );
		}
		
		static N::Volume_Container ItemSequence()  { return N::Volumes(); }
		
		static FSNode ConvertToFSNode( N::FSVolumeRefNum vRefNum )
		{
			FSSpec volume = N::FSMakeFSSpec( vRefNum, N::FSDirID( long( fsRtDirID ) ), "\p" );
			
			std::string name = UnixFromMacName( NN::Convert< std::string >( volume.name ) );
			
			FSTreePtr tree( new FSTree_FSSpec( volume ) );
			
			return FSNode( name, tree );
		}
	};
	
	typedef FSTree_Special< Volumes_Details > FSTree_Volumes;
	
	
	struct proc_Details
	{
		static FSTreePtr Lookup( const std::string& name )
		{
			pid_t pid = std::atoi( name.c_str() );
			
			return FSTreePtr( new FSTree_pid( pid ) );
		}
		
		static const GenieProcessTable& ItemSequence()  { return gProcessTable; }
		
		static FSNode ConvertToFSNode( GenieProcessTable::ProcessMap::value_type proc )
		{
			pid_t pid = proc.first;
			
			std::string name = NN::Convert< std::string >( pid );
			
			FSTreePtr tree( new FSTree_pid( pid ) );
			
			return FSNode( name, tree );
		}
	};
	
	typedef FSTree_Special< proc_Details > FSTree_proc;
	
	
	static FSTreePtr MakeFSRoot()
	{
		FSTree_FSSpec* tree = NULL;
		
		FSTreePtr result( tree = new FSTree_FSSpec( FindJDirectory() & "" ) );
		
		FSTreePtr users( new FSTree_FSSpec( N::RootDirectory( N::BootVolume() ) & "Users" ) );
		
		tree->Map( "Users",   users );
		tree->Map( "Volumes", FSTreePtr( new FSTree_Volumes() ) );
		
		tree->Map( "proc", GetSingleton< FSTree_proc >() );
		tree->Map( "dev",  GetSingleton< FSTree_dev  >() );
		
		return result;
	}
	
	
	static FSTreePtr FSNull()
	{
		static FSTreePtr null = FSTreePtr( new FSTree_Null() );
		
		return null;
	}
	
	FSTreePtr FSRoot()
	{
		static FSTreePtr root = MakeFSRoot();
		
		return root;
	}
	
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item )
	{
		return FSTreePtr( new FSTree_FSSpec( item ) );
	}
	
	
	void FSTree::ChangeMode( mode_t mode ) const
	{
		P7::ThrowErrno( EPERM );
	}
	
	void FSTree::Delete() const
	{
		P7::ThrowErrno( EPERM );
	}
	
	
	boost::shared_ptr< IOHandle > FSTree::Open( OpenFlags flags, mode_t mode ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		if ( creating && excluding )
		{
			P7::ThrowErrno( EPERM );
		}
		
		return Open( flags );
	}
	
	boost::shared_ptr< IOHandle > FSTree::Open( OpenFlags flags ) const
	{
		P7::ThrowErrno( ENOENT );  // Assume read attempt if no mode
		
		// Not reached
		return boost::shared_ptr< IOHandle >();
	}
	
	void FSTree::Exec( const char* const argv[], const char* const envp[] ) const
	{
		P7::ThrowErrno( ENOEXEC );
	}
	
	
	void FSTree::CreateDirectory( mode_t mode ) const
	{
		P7::ThrowErrno( EPERM );
	}
	
	FSTreePtr FSTree::Lookup( const std::string& name ) const
	{
		P7::ThrowErrno( ENOENT );
		
		// Not reached
		return FSTreePtr();
	}
	
	FSIteratorPtr FSTree::Iterate() const
	{
		P7::ThrowErrno( ENOENT );
		
		// Not reached
		return FSIteratorPtr();
	}
	
	
	FSTreePtr FSTree_Directory::Lookup( const std::string& name ) const
	{
		if ( name == "." )
		{
			return Self();
		}
		else if ( name == ".." )
		{
			return Parent();
		}
		
		return Lookup_Child( name );
	}
	
	
	void FSTree_Mappable::Map( const std::string& name, FSTreePtr tree )
	{
		mappings[ name ] = tree;
	}
	
	FSTreePtr FSTree_Mappable::Lookup_Child( const std::string& name ) const
	{
		FSTreePtr result = Lookup_Mapping( name );
		
		if ( result == NULL )
		{
			result = Lookup_Regular( name );
		}
		
		return result;
	}
	
	FSTreePtr FSTree_Mappable::Lookup_Mapping( const std::string& name ) const
	{
		Mappings::const_iterator found = mappings.find( name );
		
		return found != mappings.end() ? found->second : FSTreePtr();
	}
	
	
	static FSNode MakeFSNode_Virtual( const std::pair< const std::string, FSTreePtr >& node )
	{
		const std::string& name( node.first );
		const FSTreePtr  & tree( node.second );
		
		return FSNode( name, tree );
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
	
	
	void FSTree_RsrcFile::Stat( struct ::stat& sb ) const
	{
		StatFile( fileSpec, &sb, true );
	}
	
	boost::shared_ptr< IOHandle > FSTree_RsrcFile::Open( OpenFlags flags ) const
	{
		const bool rsrcFork = true;
		
		return OpenFSSpec( fileSpec, flags, rsrcFork );
	}
	
	
	FSTreePtr FSTree_FSSpec::Self() const
	{
		return FSTreePtr( new FSTree_FSSpec( *this ) );
	}
	
	FSTreePtr FSTree_FSSpec::Parent() const
	{
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
			return FSTreePtr( new FSTree_RsrcFile( fileSpec ) );
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
	
	
	void FSTree_Virtual::IterateIntoCache( FSTreeCache& cache ) const
	{
		std::transform( mappings.begin(),
		                mappings.end(),
		                std::back_inserter( cache ),
		                std::ptr_fun( MakeFSNode_Virtual ) );
	}
	
	
	boost::shared_ptr< IOHandle > FSTree_Device::Open( OpenFlags flags ) const
	{
		return GetSimpleDeviceHandle( deviceName );
	}
	
	
	FSTree_dev::FSTree_dev()
	{
		Map( "null", FSTreePtr( new FSTree_Device( "null" ) ) );
		Map( "zero", FSTreePtr( new FSTree_Device( "zero" ) ) );
		Map( "console", FSTreePtr( new FSTree_Device( "console" ) ) );
	}
	
	
	FSTreePtr FSTree_pid::Self() const
	{
		return FSTreePtr( new FSTree_pid( *this ) );
	}
	
	FSTreePtr FSTree_pid::Parent() const
	{
		return FSTreePtr( new FSTree_proc() );
	}
	
	void FSTree_pid::Stat( struct ::stat& sb ) const
	{
		StatGeneric( &sb );
	}
	
	FSTreePtr FSTree_pid::Lookup_Child( const std::string& name ) const
	{
		pid_t pid = std::atoi( name.c_str() );
		
		return GetSingleton< FSTree_Null >();
	}
	
	void FSTree_pid::IterateIntoCache( FSTreeCache& cache ) const
	{
		//
	}
	
}

