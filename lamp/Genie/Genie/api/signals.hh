/*
	signals.hh
	----------
*/

#ifndef GENIE_API_SIGNALS_HH
#define GENIE_API_SIGNALS_HH

// relix-kernel
#include "relix/api/broken_pipe.hh"


namespace Genie
{
	
	bool check_signals( bool may_throw );
	
	using relix::broken_pipe;
	
}

#endif

