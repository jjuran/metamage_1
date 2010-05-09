/*	==============
 *	FSTree_Proc.hh
 *	==============
 */

#ifndef GENIE_FILESYSTEM_FSTREE_PROC_HH
#define GENIE_FILESYSTEM_FSTREE_PROC_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_proc( const FSTreePtr& parent, const plus::string& name );
	
}

#endif

