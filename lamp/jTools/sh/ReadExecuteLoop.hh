// ==================
// ReadExecuteLoop.hh
// ==================

#ifndef READEXECUTELOOP_HH
#define READEXECUTELOOP_HH

// poseven
#include "poseven/types/fd_t.hh"
#include "poseven/types/wait_t.hh"


namespace tool
{
	
	enum PromptLevel
	{
		kPS1 = 1,
		kPS2 = 2,
		kPS3 = 3,
		kPS4 = 4
	};
	
	void SetPromptLevel( PromptLevel level );
	
	poseven::wait_t ReadExecuteLoop( poseven::fd_t  fd,
	                                 bool           prompts );
	
}

#endif

