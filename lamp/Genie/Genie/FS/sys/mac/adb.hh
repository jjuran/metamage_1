/*
	Genie/FS/sys/mac/adb.hh
	-----------------------
*/

#ifndef GENIE_FS_SYS_MAC_ADB_HH
#define GENIE_FS_SYS_MAC_ADB_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_adb( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
}

#endif

