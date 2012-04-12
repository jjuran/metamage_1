/*
	Genie/FS/sys/mac/rom.hh
	-----------------------
*/

#ifndef GENIE_FS_SYS_MAC_ROM_HH
#define GENIE_FS_SYS_MAC_ROM_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_rom( const FSTree*        parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
}

#endif

