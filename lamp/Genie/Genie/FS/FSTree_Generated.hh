/*	===================
 *	FSTree_Generated.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_GENERATED_HH
#define GENIE_FILESYSTEM_FSTREE_GENERATED_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	typedef plus::string (*Generated_ReadHook)( const FSTree* parent, const plus::string& name );
	
	
	// Can be used in premapped directory maps
	
	FSTreePtr new_generated( const FSTree*        parent,
	                         const plus::string&  name,
	                         const void*          params );
	
	
	inline FSTreePtr New_FSTree_Generated( const FSTree*        parent,
	                                       const plus::string&  name,
	                                       Generated_ReadHook   get_hook )
	{
		return new_generated( parent, name, (void*) get_hook );
	}
	
}

#endif

