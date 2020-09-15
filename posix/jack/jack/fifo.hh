/*
	fifo.hh
	-------
*/

#ifndef JACK_FIFO_HH
#define JACK_FIFO_HH

// POSIX
#include <sys/types.h>


namespace jack
{
	
	int fifo_wait( const char* path );
	
	int fifo_ready( const char* path, mode_t mode = 0 );
	
}

#endif
