/*
	fade.hh
	-------
*/

#ifndef AMICUS_FADE_HH
#define AMICUS_FADE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// iota
#include "iota/class.hh"


namespace amicus
{

class transition_fader
{
	NON_COPYABLE( transition_fader )
	
	private:
		CGDisplayFadeReservationToken token;
		
		void release();
		void acquire();
	
	public:
		transition_fader() : token()  {}
		
		~transition_fader()  { release(); }
		
		void begin();
		void end();
};

}

#endif
