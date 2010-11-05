// ==================
// ReadExecuteLoop.hh
// ==================

#ifndef READEXECUTELOOP_HH
#define READEXECUTELOOP_HH

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif
#ifndef POSEVEN_TYPES_WAIT_T_HH
#include "poseven/types/wait_t.hh"
#endif


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

