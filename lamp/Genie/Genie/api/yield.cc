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
#include "Genie/scheduler.hh"
#include "Genie/api/signals.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	bool yield( bool may_throw )
	{
		mark_process_inactive( current_process().gettid() );
		
		current_process().Yield();
		
		return check_signals( may_throw );
	}
	
	void try_again( bool nonblocking )
	{
		if ( nonblocking )
		{
			p7::throw_errno( EAGAIN );
		}
		
		yield( true );  // throw caught signals
	}
	
}

