/*
	sigset_t.hh
	-----------
*/

#ifndef RELIX_SIGNAL_SIGSET_T_HH
#define RELIX_SIGNAL_SIGSET_T_HH

#ifndef __RELIX__
#include <signal.h>
#endif


namespace relix
{
	
#ifdef __RELIX__
	
	typedef unsigned long sigset_t;
	
#else
	
	using ::sigset_t;
	
#endif
	
}

#endif

