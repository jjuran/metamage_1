/*
	BitMap.hh
	---------
*/

#ifndef CHIAROSCURO_TYPES_BITMAP_HH
#define CHIAROSCURO_TYPES_BITMAP_HH

#ifndef __QUICKDRAW__

struct Rect
{
	short top;
	short left;
	short bottom;
	short right;
};

struct BitMap
{
	char* baseAddr;
	short rowBytes;
	Rect  bounds;
};

#endif

#endif
