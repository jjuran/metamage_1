/*
	yield.cc
	--------
*/

#include "Genie/api/yield.hh"

// Standard C
#include <errno.h>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/api/signals.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void yield()
	{
		current_process().Yield();
	}
	
	void try_again( bool nonblocking )
	{
		if ( nonblocking )
		{
			p7::throw_errno( EAGAIN );
		}
		
		yield();
		
		check_signals( true );  // throw caught signals
	}
	
}

