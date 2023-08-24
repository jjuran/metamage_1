/*
	Timer.hh
	--------
*/

#ifndef TIMER_HH
#define TIMER_HH

// mac-sys-utils
#include "mac_sys/clock.hh"


const int fps = 15;

#ifdef __MC68K__
	
	namespace monotonic_clock = mac::sys::tick_clock;
	
#else
	
	namespace monotonic_clock = mac::sys::microsecond_clock;
	
#endif

const monotonic_clock::clock_t clock_period =
	monotonic_clock::clocks_per_kilosecond / 1000 / fps;

class timer
{
	private:
		typedef monotonic_clock::clock_t clock_t;
		
		clock_t its_pulse_interval;
		clock_t its_next_pulse;
		bool it_is_paused;
	
	public:
		static const clock_t arbitrarily_long = clock_t( -1 );
		
		static void get_clock( clock_t* t )  { monotonic_clock::get( t ); }
		
		timer() : its_next_pulse(), it_is_paused( true )
		{
			its_pulse_interval = clock_period;  // e.g. 4 ticks for 15fps
		}
		
		void start()  { play();  its_next_pulse += its_pulse_interval; }
		
		void play()  { it_is_paused = false;  get_clock( &its_next_pulse ); }
		void pause()  { it_is_paused = true; }
		
		bool pulse();
		
		bool paused() const  { return it_is_paused; }
		
		clock_t clocks_until_next_pulse() const
		{
			if ( paused() )
			{
				return arbitrarily_long;
			}
			
			clock_t t;
			get_clock( &t );
			
			if ( t >= its_next_pulse )
			{
				return 0;
			}
			
			return its_next_pulse - t;
		}
};

extern timer animation_timer;

#endif
