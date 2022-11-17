/*
	LEDs.hh
	-------
*/

#ifndef MACADB_LEDS_HH
#define MACADB_LEDS_HH

// mac-adb-utils
#include "mac_adb/register.hh"


namespace mac {
namespace adb {
	
	OSErr get_LEDs( address_t address );
	OSErr set_LEDs( address_t address, Byte led_bits );
	
}
}

#endif
