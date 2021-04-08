/*
	display.hh
	----------
*/

#ifndef DISPLAY_HH
#define DISPLAY_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


struct CGDisplayCapture_Failed {};

class display_capture
{
	private:
		CGDirectDisplayID  its_id;
		CGContextRef       its_context;
		CGrafPtr           its_port;
		
		// non-copyable
		display_capture           ( const display_capture& );
		display_capture& operator=( const display_capture& );
	
	public:
		display_capture( CGDirectDisplayID id = CGMainDisplayID() );
		~display_capture();
		
		CGDirectDisplayID id() const  { return its_id; }
		CGContextRef context() const  { return its_context; }
		CGrafPtr     port   () const  { return its_port;    }
};

#endif
