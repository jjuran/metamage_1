// signal.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_SIGNAL_HH
#define POSEVEN_FUNCTIONS_SIGNAL_HH

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/signo_t.hh"


namespace poseven
{
	
	typedef void (*signal_handler_t)(int);
	
	static const signal_handler_t sig_dfl = SIG_DFL;
	static const signal_handler_t sig_ign = SIG_IGN;
	static const signal_handler_t sig_err = SIG_ERR;
	
	inline signal_handler_t throw_posix_result( signal_handler_t result )
	{
		if ( (long) result < 0 )
		{
			throw_errno_internal( errno );
		}
		
		return result;
	}
	
	inline void signal( signo_t signo, signal_handler_t handler )
	{
		throw_posix_result( ::signal( signo, handler ) );
	}
	
}

#endif

