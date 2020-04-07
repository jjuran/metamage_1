// signo_t.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_SIGNO_T_HH
#define POSEVEN_TYPES_SIGNO_T_HH

// Standard C
#include <signal.h>

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace poseven
{
	
	enum signo_t
	{
		signo_t_min = nucleus::enumeration_traits< int >::min,
		
		sigio = SIGIO,
		
		sigbus = SIGBUS,
		sigfpe = SIGFPE,
		sighup = SIGHUP,
		sigill = SIGILL,
		sigint = SIGINT,
		sigurg = SIGURG,
		
		sigabrt = SIGABRT,
		sigalrm = SIGALRM,
		sigchld = SIGCHLD,
		sigcont = SIGCONT,
		sigkill = SIGKILL,
		sigpipe = SIGPIPE,
		sigquit = SIGQUIT,
		sigsegv = SIGSEGV,
		sigstop = SIGSTOP,
		sigterm = SIGTERM,
		sigtrap = SIGTRAP,
		sigtstp = SIGTSTP,
		sigttin = SIGTTIN,
		sigttou = SIGTTOU,
		sigusr1 = SIGUSR1,
		sigusr2 = SIGUSR2,
		sigxcpu = SIGXCPU,
		sigxfsz = SIGXFSZ,
		
	#ifdef SIGSYS
		
		// XSI
		sigsys = SIGSYS,
		
	#endif
		
	#ifdef SIGVTALRM
		
		// XSI
		sigvtalrm = SIGVTALRM,
		
	#endif
		
	#ifdef SIGPROF
		
		// OB XSI
		sigprof = SIGPROF,
		
	#endif
		
	#ifdef SIGPWR
		
		// non-POSIX
		sigpwr = SIGPWR,
		
	#endif
		
	#ifdef SIGSTKFLT
		
		// non-POSIX
		sigstkflt = SIGSTKFLT,
		
	#endif
		
	#ifdef SIGWINCH
		
		// non-POSIX
		sigwinch = SIGWINCH,
		
	#endif
		
		signo_t_max = nucleus::enumeration_traits< int >::max
	};
	
}

#endif
