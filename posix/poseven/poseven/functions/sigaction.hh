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
#ifndef POSEVEN_TYPES_SIGNO_T_HH
#include "poseven/types/signo_t.hh"
#endif


namespace poseven
{
	
	typedef void (*sigaction_handler_t)(int);
	
	static const sigaction_handler_t sig_dfl = SIG_DFL;
	static const sigaction_handler_t sig_ign = SIG_IGN;
	static const sigaction_handler_t sig_err = SIG_ERR;
	
	
	void sigaction( signo_t                  signo,
	                struct sigaction const&  new_action,
	                struct sigaction      &  old_action );
	
	void sigaction( signo_t                  signo,
	                const struct sigaction&  new_action );
	
	inline void sigaction( signo_t              signo,
	                       sigaction_handler_t  handler )
	{
		struct sigaction action = { 0 };
		
		action.sa_handler = handler;
		
		sigaction( signo, action );
	}
	
}

#endif
