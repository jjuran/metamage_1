/*
	ADBKeyboard.hh
	--------------
*/

#ifndef PEDESTAL_ADBKEYBOARD_HH
#define PEDESTAL_ADBKEYBOARD_HH


namespace Pedestal
{
	
	typedef unsigned char Byte;
	
	extern bool gKeyboardConfigured;
	
	void ConfigureKeyboard( Byte address, bool active, bool capsLock_on );
	
}

#endif
