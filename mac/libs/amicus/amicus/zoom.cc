/*
	zoom.cc
	-------
*/

#include "amicus/zoom.hh"


namespace amicus
{

unsigned command_ID_for_zoom_index( int zoom )
{
	if ( zoom == 1 )
	{
		return ' 50%';
	}
	
	unsigned command_ID = ('0' + (zoom >> 1)) << 24
	                    | "05"[ zoom & 1 ]    << 16
	                    | '0'                 <<  8
	                    | '%';
	
	return command_ID;
}

}
