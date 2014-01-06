/*	===================
 *	FSTree_Generated.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_GENERATED_HH
#define GENIE_FILESYSTEM_FSTREE_GENERATED_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace vfs
{
	
	typedef plus::string (*generated_file_hook)( const node* parent, const plus::string& name );
	
	// Can be used in premapped directory maps
	
	node_ptr new_generated( const node*          parent,
	                        const plus::string&  name,
	                        const void*          params );
	
}

namespace Genie
{
	
	typedef plus::string (*Generated_ReadHook)( const FSTree* parent, const plus::string& name );
	
	
	using vfs::new_generated;
	
	
	inline FSTreePtr New_FSTree_Generated( const FSTree*        parent,
	                                       const plus::string&  name,
	                                       Generated_ReadHook   get_hook )
	{
		return new_generated( parent, name, (void*) get_hook );
	}
	
}

#endif

