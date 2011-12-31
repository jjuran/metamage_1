/*
	Genie/FS/sys/mac/drive.hh
	-------------------------
*/

#ifndef GENIE_FS_SYS_MAC_DRIVE_HH
#define GENIE_FS_SYS_MAC_DRIVE_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_drive( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    const void*          args );
	
}

#endif

