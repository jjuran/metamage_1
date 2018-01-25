/*
	unstoppable.hh
	--------------
*/

#ifndef RELIX_SIGNAL_UNSTOPPABLE_HH
#define RELIX_SIGNAL_UNSTOPPABLE_HH

// relix-kernel
#include "relix/signal/sigset_t.hh"


namespace relix
{
	
	class thread;
	
	sigset_t unstoppable_signals( const thread& target );
	
}

#endif
