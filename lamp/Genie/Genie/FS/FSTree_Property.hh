/*	==================
 *	FSTree_Property.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_PROPERTY_HH
#define GENIE_FILESYSTEM_FSTREE_PROPERTY_HH

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/property.hh"


namespace Genie
{
	
	struct undefined_property {};
	
	
	// Can be used in premapped directory maps
	
	FSTreePtr new_property( const FSTreePtr&     parent,
	                        const plus::string&  name,
	                        const void*          params );
	
}

#endif

