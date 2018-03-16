/*
	ADBKeyboard.cc
	--------------
*/

#include "Pedestal/ADBKeyboard.hh"

#if ! TARGET_API_MAC_CARBON

// Arcana
#include "ADB/KeyboardLEDs.hh"
#include "ADB/KeyboardModifiers.hh"

#endif


namespace Pedestal
{
	
	bool gKeyboardConfigured = false;
	
	
#if ! TARGET_API_MAC_CARBON
	
	namespace N = Nitrogen;
	
	void ConfigureKeyboard( signed char kbd, bool active, bool capsLock_on )
	{
		const N::ADBAddress keyboard = N::ADBAddress( kbd );
		
		SetLEDs( keyboard, (active ? 1 : 0) | (capsLock_on ? 2 : 0) );
		
		SetKeyboardModifiersDistinctness( keyboard, active );
		
		gKeyboardConfigured = active;
	}
	
#endif
	
}
