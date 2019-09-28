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
		
		// non-copyable
		display_capture           ( const display_capture& );
		display_capture& operator=( const display_capture& );
	
	public:
		display_capture( CGDirectDisplayID id = CGMainDisplayID() );
		~display_capture();
		
		CGContextRef context() const  { return its_context; }
		
		double x_scale_factor( double width, double height ) const;
		
		CGRect x_scaled_frame( double width, double height ) const;
};

#endif
