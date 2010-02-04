/*	=============
 *	LongJumper.cc
 *	=============
 */

#include "Genie/Process/LongJumper.hh"

// Debug
#include "debug/assert.hh"


namespace Genie
{
	
	void LongJumper::LongJump( int second_result )
	{
		ASSERT( its_resume_handler != 0 );  // NULL
		
		its_pad.pid = second_result;
		
		its_resume_handler( &its_pad );
	}
	
}

