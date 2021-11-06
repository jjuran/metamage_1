/*
	keymap.hh
	---------
*/

#ifndef KEYMAP_HH
#define KEYMAP_HH


extern short keymap[ 64 ];

inline
short key_def( unsigned char keycode )
{
	return keycode < 64 ? keymap[ keycode ] : 0;
}

#endif
