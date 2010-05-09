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
	
	
	FSTree_Premapped::~FSTree_Premapped()
	{
		if ( itsDestructor )
		{
			itsDestructor( static_cast< FSTree* >( this ) );
		}
	}
	
	void FSTree_Premapped::Map( const Mapping& mapping )
	{
		itsMappings[ mapping.name ] = &mapping;
	}
	
	void FSTree_Premapped::AddMappings( const Mapping* array )
	{
		while ( array->name != NULL )
		{
			Map( *array );
			
			++array;
		}
	}
	
	void FSTree_Premapped::AddMappings( const Mapping*  begin,
	                                    const Mapping*  end )
	{
		for ( const Mapping* it = begin;  it != end;  ++it )
		{
			Map( *it );
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
		Mappings::const_iterator it = itsMappings.find( name );
		
		if ( it == itsMappings.end() )
		{
			return FSNull();
		}
		
		const Function& f = it->second->f;
		
		return f( (parent ? parent : this)->Self(), name );
	}
	
	void FSTree_Premapped::IterateIntoCache( FSTreeCache& cache ) const
	{
		typedef Mappings::const_iterator Iter;
		
		for ( Iter it = itsMappings.begin();  it != itsMappings.end();  ++it )
		{
			const plus::string& name = it->first;
			
			const Function& f = it->second->f;
			
			try
			{
				if ( it->second->needs_check )
				{
					FSTreePtr file = f( Self(), name );
					
					if ( !file->Exists() )
					{
						continue;
					}
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
		FSTree_Premapped* premapped = new FSTree_Premapped( parent, name, dtor );
		
		const FSTree* raw_ptr = premapped;
		
		FSTreePtr result( raw_ptr );
		
		premapped->AddMappings( mappings );
		
		return result;
	}
	
}

