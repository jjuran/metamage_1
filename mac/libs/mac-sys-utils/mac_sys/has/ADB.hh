/*
	ADB.hh
	------
*/

#ifndef MACSYS_HAS_ADB_HH
#define MACSYS_HAS_ADB_HH

// mac-sys-utils
#include "mac_sys/trap_available.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_ADB()
	{
		enum
		{
			_CountADBs = 0xA077,
		};
		
	#if TARGET_API_MAC_CARBON
		
		return false;  // ADB API doesn't exist in Carbon
		
	#endif
		
		return trap_available( _CountADBs );
	}
	
}
}

#endif
