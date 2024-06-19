/*
	fade.hh
	-------
*/

#ifndef FADE_HH
#define FADE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


class transition_fader
{
#ifdef MAC_OS_X_VERSION_10_2
	
	private:
		CGDisplayFadeReservationToken token;
		
		void begin();
		void end();
		
		// non-copyable
		transition_fader           ( const transition_fader& );
		transition_fader& operator=( const transition_fader& );
	
	public:
		transition_fader()  { begin(); }
		
		~transition_fader()  { end(); }
	
#else
	
	public:
		transition_fader()  {}  // silence Metrowerks unused-var warning
	
#endif
};

#endif
