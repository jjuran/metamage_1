/*	===================
 *	FSTree_Directory.cc
 *	===================
 */

#include "Genie/FileSystem/FSTree_Directory.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <iterator>

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	UInt32 DecodeHex32( const char* begin, const char* end )
	{
		using BitsAndBytes::DecodeHex;
		
		std::string decoded = DecodeHex( std::string( begin, end ) );
		
		if ( decoded.size() != sizeof (UInt32) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return *(UInt32*) decoded.data();
	}
	
	
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
	
	
	FSTree_Directory::~FSTree_Directory()
	{
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
	
	
	FSTree_Mappable::~FSTree_Mappable()
	{
	}
	
	void FSTree_Mappable::Map( FSTreePtr tree )
	{
		mappings[ tree->Name() ] = tree;
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

