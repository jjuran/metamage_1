/*	=============
 *	LongJumper.cc
 *	=============
 */

#include "Genie/Process/LongJumper.hh"

// Nucleus
#include "Nucleus/NAssert.h"


namespace Genie
{
	
	void LongJumper::LongJump( int second_result ) const
	{
		ASSERT( its_long_jump != NULL );
		ASSERT( its_buffer    != NULL );
		
		its_long_jump( its_buffer, second_result );
	}
	
}

