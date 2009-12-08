/*	==================
 *	ADBKeyboardLEDs.cc
 *	==================
 */

#if !TARGET_API_MAC_CARBON

#include "ADB/KeyboardLEDs.hh"


namespace N = Nitrogen;


const UInt8 kLEDRegisterOffset = 2;
const UInt8 kLEDByteIndex      = 2;

const UInt8 kLEDValueMask = 7;


UInt8 GetLEDs( N::ADBAddress address )
{
	UInt8 ledByte = GetADBRegister( address, kLEDRegisterOffset ).buffer[ kLEDByteIndex ];
	
	UInt8 leds = ~ledByte & kLEDValueMask;
	
	return leds;
}

void SetLEDs( N::ADBAddress address, UInt8 leds )
{
	ADBRegister reg = GetADBRegister( address, kLEDRegisterOffset );
	
	UInt8 ledByte = reg.buffer[ kLEDByteIndex ] & ~kLEDValueMask;
	
	ledByte |= ~leds & kLEDValueMask;
	
	reg.buffer[ kLEDByteIndex ] = ledByte;

	SetADBRegister( address, kLEDRegisterOffset, reg );
}

#endif

