/*
	redraw_lock.cc
	--------------
*/

#include "redraw_lock.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


char* ScrnBase : 0x0824;


static inline
bool is_screen( const BitMap& bitmap )
{
	return bitmap.baseAddr == ScrnBase;
}

redraw_lock::redraw_lock( const BitMap& bitmap )
:
	its_raster_lock( is_screen( bitmap ) )
{
}

redraw_lock::redraw_lock( const BitMap& oneBits, const BitMap& twoBits )
:
	its_raster_lock( is_screen( oneBits )  ||  is_screen( twoBits ) )
{
}
