/*	=======================
 *	ADBKeyboardModifiers.cc
 *	=======================
 */

#if !TARGET_API_MAC_CARBON

#include "ADB/KeyboardModifiers.hh"


namespace N = Nitrogen;


const UInt8 kDeviceHandlerIDForNormalKeyboard                = 2;
const UInt8 kDeviceHandlerIDForKeyboardWithDistinctModifiers = 3;

bool KeyboardModifiersAreDistinct( Nitrogen::ADBAddress address )
{
	UInt8 id = GetDeviceHandlerID( address );
	
	return id != kDeviceHandlerIDForNormalKeyboard;
}

void SetKeyboardModifiersDistinctness( Nitrogen::ADBAddress address, bool distinct )
{
	const UInt8 id = distinct ? kDeviceHandlerIDForKeyboardWithDistinctModifiers
	                          : kDeviceHandlerIDForNormalKeyboard;
	
	SetDeviceHandlerID( address, id );
}

#endif

