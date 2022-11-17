/*
	ADBKeyboard.cc
	--------------
*/

#include "Pedestal/ADBKeyboard.hh"

// mac-adb-utils
#include "mac_adb/device_handler_ID.hh"
#include "mac_adb/keyboard.hh"
#include "mac_adb/LEDs.hh"


#pragma exceptions off


namespace Pedestal
{
	
	bool gKeyboardConfigured = false;
	
	
	void ConfigureKeyboard( Byte address, bool active, bool capsLock_on )
	{
		using namespace mac::adb;
		
		const address_t keyboard = address;
		
		Byte id = device_handler_ID_for_keyboard_with_remapping( active );
		
		set_LEDs( keyboard, (active ? 1 : 0) | (capsLock_on ? 2 : 0) );
		
		set_device_handler_ID( keyboard, id );
		
		gKeyboardConfigured = active;
	}
	
}
