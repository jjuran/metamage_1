/*
	Genie/FS/sys/mac/proc.hh
	------------------------
*/

#ifndef GENIE_FS_SYS_MAC_PROC_HH
#define GENIE_FS_SYS_MAC_PROC_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_proc( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args );
	
}

#endif

