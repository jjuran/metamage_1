/*	===================
 *	FSTree_Directory.cc
 *	===================
 */

#include "Genie/FS/FSTree_Directory.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/FSTreeCache.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
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
			
			return f( (parent ? parent : this)->Self(), name, it->args );
		}
		
		p7::throw_errno( ENOENT );
		
		// not reached
		return FSTreePtr();
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

