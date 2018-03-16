/*
	ADBKeyboard.hh
	--------------
*/

#ifndef PEDESTAL_ADBKEYBOARD_HH
#define PEDESTAL_ADBKEYBOARD_HH


namespace Pedestal
{
	
	extern bool gKeyboardConfigured;
	
	void ConfigureKeyboard( signed char kbd, bool active, bool capsLock_on );
	
}

#endif
