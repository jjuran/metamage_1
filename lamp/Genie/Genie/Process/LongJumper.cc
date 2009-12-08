/*	=============
 *	LongJumper.cc
 *	=============
 */

#include "Genie/Process/LongJumper.hh"

// Debug
#include "debug/assert.hh"


namespace Genie
{
	
	void LongJumper::LongJump( int second_result ) const
	{
		ASSERT( its_long_jump != 0 );  // NULL
		ASSERT( its_buffer    != 0 );  // NULL
		
		its_long_jump( its_buffer, second_result );
	}
	
}

