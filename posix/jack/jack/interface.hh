/*
	interface.hh
	------------
*/

#ifndef JACK_INTERFACE_HH
#define JACK_INTERFACE_HH

// plus
#include "plus/var_string.hh"


namespace jack
{
	
	class interface
	{
		private:
			plus::var_string  its_path;
			unsigned          its_subpath_length;
			
			const char* full_path( const char* extension );
		
		public:
			interface( const char* subpath );
			
			const char* socket_path()  { return full_path( ".socket" ); }
			const char* fifo_path  ()  { return full_path( ".fifo"   ); }
			
			
	};
	
}

#endif
