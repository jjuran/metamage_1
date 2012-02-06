/*	===================
 *	FSTree_Directory.cc
 *	===================
 */

#include "Genie/FS/FSTree_Directory.hh"

// Genie
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Null.hh"


namespace Genie
{
	
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
			return it->f( (parent ? parent : this)->Self(), name, it->args );
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
				FSTreePtr file = f( Self(), name, it->args );
				
				if ( !exists( file ) )
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

