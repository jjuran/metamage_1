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
		
		cache[0] = FSNode( Inode(),       "."  ); 
		cache[1] = FSNode( ParentInode(), ".." );
		
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
	
	FSTreePtr Premapped_Factory( const FSTreePtr&           parent,
	                             const std::string&         name,
	                             const Singleton_Mapping    mappings[],
	                             void                     (*dtor)(const FSTree*) )
	{
		FSTree_Functional< void >* raw_ptr = new FSTree_Functional< void >( parent, name, dtor );
		
		FSTreePtr result( raw_ptr );
		
		raw_ptr->AddMappings( mappings );
		
		return result;
	}
	
}

