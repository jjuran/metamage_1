/*
	display.hh
	----------
*/

#ifndef AMICUS_DISPLAY_HH
#define AMICUS_DISPLAY_HH

// Mac OS X
#ifdef __APPLE__
// Including just <CoreGraphics/CoreGraphics.h> fails in Mac OS X 10.4.
#include <ApplicationServices/ApplicationServices.h>
#endif

// iota
#include "iota/class.hh"


namespace amicus
{

struct CGDisplayCapture_Failed {};

class display_capture
{
	NON_COPYABLE( display_capture )
	
	private:
		CGDirectDisplayID  its_id;
	
	public:
		display_capture( CGDirectDisplayID id = CGMainDisplayID() );
		~display_capture();
		
		CGDirectDisplayID id() const  { return its_id; }
};

}

#endif
