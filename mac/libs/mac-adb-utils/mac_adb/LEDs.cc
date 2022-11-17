/*
	LEDs.cc
	-------
*/

#include "mac_adb/LEDs.hh"


#pragma exceptions off


namespace mac {
namespace adb {

enum
{
	kLEDRegisterIndex = 2,
	kLEDByteIndex     = 2,
	
	kLEDValueMask = 7,
};


OSErr get_LEDs( address_t address )
{
	reg_buffer reg;
	
	if ( OSErr err = get_register( reg, address, kLEDRegisterIndex ) )
	{
		return err;
	}
	
	Byte led_byte = reg.data[ kLEDByteIndex ];
	Byte led_bits = ~led_byte & kLEDValueMask;
	
	return led_bits;
}

OSErr set_LEDs( address_t address, Byte led_bits )
{
	reg_buffer reg;
	
	if ( OSErr err = get_register( reg, address, kLEDRegisterIndex ) )
	{
		return err;
	}
	
	Byte& led_byte = reg.data[ kLEDByteIndex ];
	
	led_byte = (led_byte & ~kLEDValueMask)
	         | (~led_bits & kLEDValueMask);
	
	return set_register( reg, address, kLEDRegisterIndex );
}

}
}
