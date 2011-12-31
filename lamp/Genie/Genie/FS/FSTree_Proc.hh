/*	==============
 *	FSTree_Proc.hh
 *	==============
 */

#ifndef GENIE_FILESYSTEM_FSTREE_PROC_HH
#define GENIE_FILESYSTEM_FSTREE_PROC_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_proc( const FSTreePtr&     parent,
	                           const plus::string&  name,
	                           const void*          args );
	
}

#endif

