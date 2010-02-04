/*
	vfork_context.cc
	----------------
	
	Joshua Juran
*/

#include "Genie/Process/vfork_context.hh"

// Debug
#include "debug/assert.hh"


namespace Genie
{
	
	void vfork_context::resume_vfork( int second_result )
	{
		ASSERT( its_resume_handler != 0 );  // NULL
		
		its_pad.pid = second_result;
		
		its_resume_handler( &its_pad );
	}
	
}

