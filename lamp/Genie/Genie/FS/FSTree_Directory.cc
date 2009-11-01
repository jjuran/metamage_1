/*	===================
 *	FSTree_Directory.cc
 *	===================
 */

#include "Genie/FS/FSTree_Directory.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <iterator>


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
	
	FSTreePtr FSTree_Premapped::Lookup_Child( const std::string& name ) const
	{
		Mappings::const_iterator it = itsMappings.find( name );
		
		if ( it == itsMappings.end() )
		{
			return FSNull();
		}
		
		const Function& f = it->second->f;
		
		return f( Self(), name );
	}
	
	void FSTree_Premapped::IterateIntoCache( FSTreeCache& cache ) const
	{
		typedef Mappings::const_iterator Iter;
		
		for ( Iter it = itsMappings.begin();  it != itsMappings.end();  ++it )
		{
			const std::string& name = it->first;
			
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
	                             const std::string&                 name,
	                             const FSTree_Premapped::Mapping    mappings[],
	                             void                             (*dtor)(const FSTree*) )
	{
		FSTree_Premapped* raw_ptr = new FSTree_Premapped( parent, name, dtor );
		
		FSTreePtr result( raw_ptr );
		
		raw_ptr->AddMappings( mappings );
		
		return result;
	}
	
}

