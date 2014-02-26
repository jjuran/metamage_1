/*
	root.hh
	-------
*/

#ifndef RELIX_API_ROOT
#define RELIX_API_ROOT

// vfs
#include "vfs/node_fwd.hh"
#include "vfs/functions/root.hh"


namespace relix
{
	
	inline const vfs::node* root()
	{
		return vfs::root();
	}
	
}


#endif
