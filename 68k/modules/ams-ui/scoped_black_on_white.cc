/*
	scoped_black_on_white.cc
	------------------------
*/

#include "scoped_black_on_white.hh"


scoped_black_on_white::scoped_black_on_white( GrafPtr port )
{
	its_port = port;
	
	saved_fgColor = port->fgColor;
	saved_bkColor = port->bkColor;
	
	port->fgColor = blackColor;
	port->bkColor = whiteColor;
}
