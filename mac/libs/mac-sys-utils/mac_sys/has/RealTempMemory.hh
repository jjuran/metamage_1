/*
	RealTempMemory.hh
	-----------------
*/

#ifndef MACSYS_HAS_REALTEMPMEMORY_HH
#define MACSYS_HAS_REALTEMPMEMORY_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_RealTempMemory()
	{
		enum
		{
			gestaltOSAttr         = 'os  ',
			
			gestaltRealTempMemory = 5,
		};
		
		return gestalt_bit_set( gestaltOSAttr, gestaltRealTempMemory );
	}
	
}
}

#endif
