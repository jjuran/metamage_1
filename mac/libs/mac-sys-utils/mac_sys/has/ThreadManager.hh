/*
	ThreadManager.hh
	----------------
*/

#ifndef MACSYS_HAS_THREADMANAGER_HH
#define MACSYS_HAS_THREADMANAGER_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_ThreadManager()
	{
		enum
		{
			gestaltThreadMgrAttr = 'thds',
		};
		
		return gestalt( gestaltThreadMgrAttr );
	}
	
}
}

#endif
