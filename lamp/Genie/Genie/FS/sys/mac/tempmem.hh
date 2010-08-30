/*
	Genie/FS/sys/mac/tempmem.hh
	---------------------------
*/

#ifndef GENIE_FS_SYS_MAC_TEMPMEM_HH
#define GENIE_FS_SYS_MAC_TEMPMEM_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_tempmem( const FSTreePtr&     parent,
	                                      const plus::string&  name,
	                                      const void*          args );
	
}

#endif

