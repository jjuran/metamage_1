/*
	Genie/FS/sys/mac/crm/serial.hh
	------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_CRM_SERIAL_HH
#define GENIE_FS_SYS_MAC_CRM_SERIAL_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_crm_serial( const FSTree*        parent,
	                                         const plus::string&  name,
	                                         const void*          args );
	
	
}

#endif

