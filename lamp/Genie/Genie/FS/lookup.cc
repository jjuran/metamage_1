/*
	lookup.cc
	---------
*/

#include "Genie/FS/lookup.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/parent.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	FSTreePtr lookup( const FSTree*        it,
	                  const plus::string&  name,
	                  const FSTree*        surrogate )
	{
		if ( name == "." )
		{
			return it;
		}
		else if ( name == ".." )
		{
			return parent( it );
		}
		
		if ( surrogate == NULL )
		{
			surrogate = it;
		}
		
		const node_method_set* methods = it->methods();
		
		const dir_method_set* dir_methods;
		
		if ( methods  &&  (dir_methods = methods->dir_methods) )
		{
			if ( dir_methods->lookup )
			{
				return dir_methods->lookup( it, name, surrogate );
			}
		}
		
		throw p7::errno_t( ENOTDIR );
	}
	
}

