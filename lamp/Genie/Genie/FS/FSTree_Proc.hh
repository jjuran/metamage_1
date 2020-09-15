/*	==============
 *	FSTree_Proc.hh
 *	==============
 */

#ifndef GENIE_FILESYSTEM_FSTREE_PROC_HH
#define GENIE_FILESYSTEM_FSTREE_PROC_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_proc( const vfs::node*     parent,
	                               const plus::string&  name,
	                               const void*          args );
	
}

#endif
