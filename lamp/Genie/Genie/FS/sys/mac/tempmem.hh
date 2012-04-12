/*
	Genie/FS/sys/mac/tempmem.hh
	---------------------------
*/

#ifndef GENIE_FS_SYS_MAC_TEMPMEM_HH
#define GENIE_FS_SYS_MAC_TEMPMEM_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_tempmem( const FSTree*        parent,
	                                      const plus::string&  name,
	                                      const void*          args );
	
}

#endif

