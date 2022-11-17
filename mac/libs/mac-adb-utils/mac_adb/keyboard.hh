/*
	keyboard.hh
	-----------
*/

#ifndef MACADB_KEYBOARD_HH
#define MACADB_KEYBOARD_HH


namespace mac {
namespace adb {
	
	enum
	{
		kKeyboardWithOriginalModifiers = 2,
		kKeyboardWithRemappedModifiers = 3,
	};
	
	typedef unsigned char Byte;
	
	inline
	Byte device_handler_ID_for_keyboard_with_remapping( bool remapped )
	{
		const Byte id = remapped ? kKeyboardWithRemappedModifiers
		                         : kKeyboardWithOriginalModifiers;
		
		return id;
	}
	
}
}

#endif
