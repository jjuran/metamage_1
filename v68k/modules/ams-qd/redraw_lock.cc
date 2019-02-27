/*
	redraw_lock.cc
	--------------
*/

#include "redraw_lock.hh"


char* ScrnBase : 0x0824;


redraw_lock::redraw_lock( char* baseAddr )
:
	its_raster_lock( baseAddr == ScrnBase )
{
}

redraw_lock::redraw_lock( char* baseAddr1, char* baseAddr2 )
:
	its_raster_lock( baseAddr1 == ScrnBase  ||  baseAddr2 == ScrnBase )
{
}
