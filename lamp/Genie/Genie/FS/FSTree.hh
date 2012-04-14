/*	=========
 *	FSTree.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSTREE_HH
#define GENIE_FILESYSTEM_FSTREE_HH

// vfs
#include "vfs/node.hh"
#include "vfs/functions/root.hh"

// Genie
#include "Genie/FS/FSTree_fwd.hh"
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	using vfs::node_destructor;
	using vfs::node_method_set;
	
	
	extern const FSTreePtr null_FSTreePtr;
	
	inline const FSTree* FSRoot()
	{
		return vfs::root();
	}
	
}

#endif

