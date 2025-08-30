/*
	paint_screen.hh
	---------------
*/

#ifndef PAINTSCREEN_HH
#define PAINTSCREEN_HH


struct BitMap;

typedef unsigned char  Byte;
typedef short          OSErr;

OSErr paint_screen( const Byte* filename );

#endif
