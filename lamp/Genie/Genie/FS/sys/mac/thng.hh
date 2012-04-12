/*
	Genie/FS/sys/mac/thng.hh
	------------------------
*/

#ifndef GENIE_FS_SYS_MAC_THNG_HH
#define GENIE_FS_SYS_MAC_THNG_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_thng( const FSTree*        parent,
	                                   const plus::string&  name,
	                                   const void*          args );
	
}

#endif

