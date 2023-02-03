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
	
#ifdef MAC_OS_X_VERSION_10_5
	
	CGAssociateMouseAndMouseCursorPosition( false );
	
#endif
}

display_capture::~display_capture()
{
#ifdef MAC_OS_X_VERSION_10_5
	
	CGAssociateMouseAndMouseCursorPosition( true );
	
#endif
	
	CGDisplayShowCursor( its_id );
	
	CGDisplayRelease( its_id );
}

}
