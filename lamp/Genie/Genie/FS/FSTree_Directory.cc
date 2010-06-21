/*	===================
 *	FSTree_Directory.cc
 *	===================
 */

#include "Genie/FS/FSTree_Directory.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <iterator>

// Genie
#include "Genie/FS/FSTreeCache_Impl.hh"
#include "Genie/FS/FSTree_Null.hh"


namespace Genie
{
	
	typedef boost::shared_ptr< const FSTreeCache > FSTreeCachePtr;
	
	class FSIterator_Cache : public FSIterator
	{
		private:
			FSTreeCachePtr  contents;
			std::size_t     nextOffset;
		
		public:
			FSIterator_Cache( const FSTreeCachePtr& cache ) : contents( cache ), nextOffset( 0 )  {}
			
			FSNode Get() const
			{
				const FSTreeCache_Impl* impl = static_cast< const FSTreeCache_Impl* >( contents.get() );
				
				return nextOffset < impl->size() ? impl->at( nextOffset )
				                                 : FSNode();
			}
			
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
		FSTreeCache_Impl cache;
		
		cache.push_back( FSNode( Inode(),       "."  ) );
		cache.push_back( FSNode( ParentInode(), ".." ) );
		
		IterateIntoCache( cache );
		
		FSTreeCache_Impl* newCache = new FSTreeCache_Impl();
		
		FSTreeCachePtr cachePtr( newCache );
		
		using std::swap;
		
		swap( cache, *newCache );
		
		return FSIteratorPtr( new FSIterator_Cache( cachePtr ) );
	}
	
	FSTreePtr FSTree_Directory::Lookup( const plus::string& name, const FSTree* parent ) const
	{
		if ( name == "." )
		{
			return Self();
		}
		else if ( name == ".." )
		{
			return Parent();
		}
		
		return Lookup_Child( name, parent );
	}
	
	
	const FSTree_Premapped::Mapping
	//
	FSTree_Premapped::empty_mappings[] = { { NULL, NULL } };
	
	
	static const FSTree_Premapped::Mapping*
	//
	find_mapping( const FSTree_Premapped::Mapping* mappings, const plus::string& name )
	{
		for ( const FSTree_Premapped::Mapping* it = mappings;  it->name;  ++it )
		{
			if ( it->name == name )
			{
				return it;
			}
		}
		
		return NULL;
	}
	
	FSTree_Premapped::~FSTree_Premapped()
	{
		if ( itsDestructor )
		{
			itsDestructor( static_cast< FSTree* >( this ) );
		}
	}
	
	void FSTree_Premapped::Delete() const
	{
		if ( itsDestructor )
		{
			itsDestructor( static_cast< const FSTree* >( this ) );
		}
	}
	
	FSTreePtr FSTree_Premapped::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		if ( const Mapping* it = find_mapping( itsMappings, name ) )
		{
			const Function& f = it->f;
			
			return f( (parent ? parent : this)->Self(), name );
		}
		
		return FSNull();
	}
	
	void FSTree_Premapped::IterateIntoCache( FSTreeCache& cache ) const
	{
		for ( const Mapping* it = itsMappings;  it->name != NULL;  ++it )
		{
			const plus::string& name = it->name;
			
			const Function& f = it->f;
			
			try
			{
				FSTreePtr file = f( Self(), name );
				
				if ( !file->Exists() )
				{
					continue;
				}
				
				ino_t inode = 0;
				
				cache.push_back( FSNode( inode, name ) );
			}
			catch ( ... )
			{
			}
		}
	}
	
	
	FSTreePtr Premapped_Factory( const FSTreePtr&                   parent,
	                             const plus::string&                name,
	                             const FSTree_Premapped::Mapping    mappings[],
	                             void                             (*dtor)(const FSTree*) )
	{
		return FSTreePtr( new FSTree_Premapped( parent, name, mappings, dtor ) );
	}
	
}

