/*
	Genie/FS/sys/mac/soundin.hh
	---------------------------
*/

#ifndef GENIE_FS_SYS_MAC_SOUNDIN_HH
#define GENIE_FS_SYS_MAC_SOUNDIN_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_soundin( const FSTreePtr&     parent,
	                                      const plus::string&  name,
	                                      const void*          args );
	
}

#endif

