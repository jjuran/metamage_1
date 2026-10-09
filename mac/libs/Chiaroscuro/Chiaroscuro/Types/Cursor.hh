/*
	Cursor.hh
	---------
*/

#ifndef CHIAROSCURO_TYPES_CURSOR_HH
#define CHIAROSCURO_TYPES_CURSOR_HH

#ifndef __QUICKDRAW__

typedef short Bits16[ 16 ];

struct Point
{
	short v;
	short h;
};

struct Cursor
{
	Bits16 data;
	Bits16 mask;
	Point  hotspot;
};

#endif

#endif
