/*
	parameter_block.hh
	------------------
*/

#ifndef CTHREAD_PARAMETERBLOCK_HH
#define CTHREAD_PARAMETERBLOCK_HH

// cthread
#include "cthread/types.hh"


namespace cthread
{
	
	struct parameter_block
	{
		start_proc   start;
		void*        param;
		
		switch_proc  switch_in;
		switch_proc  switch_out;
		
		const void*  stack_bottom;
		const void*  stack_limit;
	};
	
}

#endif
