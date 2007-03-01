/*	===================
 *	FSTree_Directory.cc
 *	===================
 */

#include "Genie/FileSystem/FSTree_Directory.hh"

// POSIX
#include "sys/stat.h"

// Nucleus
//#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	
	
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
	
	
	class FSTree_Null : public FSTree
	{
		public:
			bool Exists() const  { return false; }
			bool IsFile() const  { return false; }
			
			FSTreePtr Parent() const  { P7::ThrowErrno( ENOENT );  return FSTreePtr(); }
	};
	
	
	FSTreePtr FSNull()
	{
		static FSTreePtr null = FSTreePtr( new FSTree_Null() );
		
		return null;
	}
	
	
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
	
	
	void FSTree_Virtual::IterateIntoCache( FSTreeCache& cache ) const
	{
		std::transform( mappings.begin(),
		                mappings.end(),
		                std::back_inserter( cache ),
		                std::ptr_fun( MakeFSNode_Virtual ) );
	}
	
}

