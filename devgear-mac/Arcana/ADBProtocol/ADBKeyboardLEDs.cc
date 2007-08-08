/*	==================
 *	ADBKeyboardLEDs.cc
 *	==================
 */

#include "ADBKeyboardLEDs.hh"


namespace N = Nitrogen;


const UInt8 kLEDRegisterIndex = 2;

const UInt8 kLEDValueMask = 7;


UInt8 GetLEDs( N::ADBAddress address )
{
	UInt8 leds = GetADBRegister( address, kLEDRegisterIndex ).buffer[2] & kLEDValueMask;
	
	return leds;
}

void SetLEDs( N::ADBAddress address, UInt8 leds )
{
	ADBRegister reg2 = GetADBRegister( address, kLEDRegisterIndex );
	
	reg2.buffer[2] &= ~kLEDValueMask;
	
	reg2.buffer[2] |= leds & kLEDValueMask;

	SetADBRegister( address, kLEDRegisterIndex, reg2 );
}

