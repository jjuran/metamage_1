/*
	empty_signal_handler.cc
	-----------------------
*/

#include "varyx/posix/empty_signal_handler.hh"

// Standard C
#include <signal.h>
#include <stdlib.h>


namespace varyx {
namespace posix {

static
void empty_signal_handler( int )
{
	// empty
}

void install_empty_signal_handler( int signum )
{
	struct sigaction action = { 0 };
	action.sa_handler = &empty_signal_handler;
	
	int nok = sigaction( signum, &action, NULL );
	
	if ( nok )
	{
		abort();
	}
}

bool verify_empty_signal_handler( int signum )
{
	struct sigaction sig;
	
	int nok = sigaction( signum, NULL, &sig );
	
	if ( nok )
	{
		return false;
	}
	
	return sig.sa_handler == &empty_signal_handler;
}

}
}
