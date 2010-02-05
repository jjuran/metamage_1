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
		const _resume_handler_t resume = its_pad.resume_handler;
		
		ASSERT( resume != NULL );
		
		its_pad.pid = second_result;
		
		resume( &its_pad );
	}
	
	void vfork_context::set_resume_handler( const _vfork_pad* pad )
	{
		its_pad = *pad;
	}
	
}

