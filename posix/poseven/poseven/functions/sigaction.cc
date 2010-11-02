/*
	sigaction.cc
	------------
*/

#include "poseven/functions/sigaction.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void sigaction( signo_t                  signo,
	                struct sigaction const&  new_action,
	                struct sigaction      &  old_action )
	{
		throw_posix_result( ::sigaction( signo, &new_action, &old_action ) );
	}
	
	void sigaction( signo_t                  signo,
	                const struct sigaction&  new_action )
	{
		throw_posix_result( ::sigaction( signo, &new_action, 0 ) );  // NULL
	}
	
}

