/*
	notify.cc
	---------
*/

#include "update/notify.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdlib.h>


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k   {
namespace update {

#ifndef __RELIX__

static const char* update_fifo = getenv( "GRAPHICS_UPDATE_SIGNAL_FIFO" );

void notify()
{
	if ( update_fifo )
	{
		close( open( update_fifo, O_RDONLY | O_NONBLOCK ) );
	}
}

#endif

}  // namespace update
}  // namespace v68k
