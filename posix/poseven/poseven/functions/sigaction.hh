// sigaction.hh
// ------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2010 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_SIGACTION_HH
#define POSEVEN_FUNCTIONS_SIGACTION_HH

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/signo_t.hh"


namespace poseven
{
	
	typedef void (*sigaction_handler_t)(int);
	
	static const sigaction_handler_t sig_dfl = SIG_DFL;
	static const sigaction_handler_t sig_ign = SIG_IGN;
	static const sigaction_handler_t sig_err = SIG_ERR;
	
	
	inline void sigaction( signo_t                  signo,
	                       struct sigaction const&  new_action,
	                       struct sigaction      &  old_action )
	{
		throw_posix_result( ::sigaction( signo, &new_action, &old_action ) );
	}
	
	inline void sigaction( signo_t                  signo,
	                       const struct sigaction&  new_action )
	{
		throw_posix_result( ::sigaction( signo, &new_action, NULL ) );
	}
	
	inline void sigaction( signo_t              signo,
	                       sigaction_handler_t  handler )
	{
		struct sigaction action = { handler, 0, 0 };
		
		sigaction( signo, action );
	}
	
}

#endif

