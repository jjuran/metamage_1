/*
	FSSpec_calls.hh
	---------------
*/

#ifndef MACSYS_HAS_FSSPECCALLS_HH
#define MACSYS_HAS_FSSPECCALLS_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_FSSpec_calls()
	{
		enum
		{
			gestaltFSAttr         = 'fs  ',
			
			gestaltHasFSSpecCalls = 1,
		};
		
		return gestalt_bit_set( gestaltFSAttr, gestaltHasFSSpecCalls );
	}
	
}
}

#endif
