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


namespace amicus
{

struct CGDisplayCapture_Failed {};

class display_capture
{
	private:
		CGDirectDisplayID  its_id;
		
		// non-copyable
		display_capture           ( const display_capture& );
		display_capture& operator=( const display_capture& );
	
	public:
		display_capture( CGDirectDisplayID id = CGMainDisplayID() );
		~display_capture();
		
		CGDirectDisplayID id() const  { return its_id; }
};

}

#endif
