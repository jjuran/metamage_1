/*	======================
 *	FSTree_sys_mac_proc.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACPROC_HH
#define GENIE_FILESYSTEM_FSTREESYSMACPROC_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_proc( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args );
	
}

#endif

