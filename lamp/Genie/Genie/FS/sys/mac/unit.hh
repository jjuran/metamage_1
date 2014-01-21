/*
	Genie/FS/sys/mac/unit.hh
	------------------------
*/

#ifndef GENIE_FS_SYS_MAC_UNIT_HH
#define GENIE_FS_SYS_MAC_UNIT_HH

// plus
#include "plus/string_fwd.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_unit( const FSTree*        parent,
	                                   const plus::string&  name,
	                                   const void*          args );
	
}

#endif

