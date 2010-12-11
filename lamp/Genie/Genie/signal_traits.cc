/*
	signal_traits.cc
	----------------
*/

#include "Genie/signal_traits.hh"


namespace Genie
{
	
	const signal_traits global_signal_traits[] =
	{
		signal_null,            // 0
		signal_terminate,       // SIGHUP
		signal_terminate,       // SIGINT
		signal_terminate_core,  // SIGQUIT
		signal_terminate_core,  // SIGILL
		signal_terminate_core,  // SIGTRAP
		signal_terminate_core,  // SIGABRT
		signal_terminate_core,  // SIGBUS
		signal_terminate_core,  // SIGFPE
		signal_sigkill,         // SIGKILL
		signal_terminate,       // SIGUSR1
		signal_terminate_core,  // SIGSEGV
		signal_terminate,       // SIGUSR2
		signal_terminate,       // SIGPIPE
		signal_terminate,       // SIGALRM
		signal_terminate,       // SIGTERM
		signal_terminate_core,  // SIGSTKFLT
		signal_discard,         // SIGCHLD
		signal_continue,        // SIGCONT
		signal_sigstop,         // SIGSTOP
		signal_stop,            // SIGTSTP
		signal_stop,            // SIGTTIN
		signal_stop,            // SIGTTOU
		signal_discard,         // SIGURG
		signal_terminate,       // SIGXCPU
		signal_terminate,       // SIGXFSZ
		signal_terminate,       // SIGVTALRM
		signal_terminate,       // SIGPROF
		signal_discard,         // SIGWINCH
		signal_discard,         // SIGIO
		signal_discard,         // SIGPWR  // FIXME
		signal_terminate_core   // SIGSYS
	};
	
}

