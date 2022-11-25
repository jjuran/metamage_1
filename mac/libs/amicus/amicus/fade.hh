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


namespace amicus
{

class transition_fader
{
	private:
		CGDisplayFadeReservationToken token;
		
		void release();
		void acquire();
		
		// non-copyable
		transition_fader           ( const transition_fader& );
		transition_fader& operator=( const transition_fader& );
	
	public:
		transition_fader() : token()  {}
		
		~transition_fader()  { release(); }
		
		void begin();
		void end();
};

}

#endif
