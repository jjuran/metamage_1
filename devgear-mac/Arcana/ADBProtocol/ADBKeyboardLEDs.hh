/*	==================
 *	ADBKeyboardLEDs.hh
 *	==================
 */

#ifndef ADBKEYBOARDLEDS_HH
#define ADBKEYBOARDLEDS_HH

// Arcana
#include "ADBProtocol.hh"


UInt8 GetLEDs( Nitrogen::ADBAddress address );

void SetLEDs( Nitrogen::ADBAddress address, UInt8 leds );

#endif

