/*
	display.cc
	----------
*/

#include "amicus/display.hh"


namespace amicus
{

display_capture::display_capture( CGDirectDisplayID id ) : its_id( id )
{
	if ( CGError err = CGDisplayCapture( its_id ) )
	{
		throw CGDisplayCapture_Failed();
	}
	
	CGDisplayHideCursor( its_id );
}

display_capture::~display_capture()
{
	CGDisplayShowCursor( its_id );
	
	CGDisplayRelease( its_id );
}

}
