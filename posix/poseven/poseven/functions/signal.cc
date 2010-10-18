/*
	signal.cc
	---------
*/

#include "poseven/functions/signal.hh"


namespace poseven
{
	
	void signal( signo_t signo, signal_handler_t handler )
	{
		throw_posix_result( ::signal( signo, handler ) );
	}
	
}

