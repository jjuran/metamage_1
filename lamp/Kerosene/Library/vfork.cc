/*	========
 *	vfork.cc
 *	========
 */

#include "vfork.h"

// Kerosene
#include "environ.hh"


#pragma exceptions off


// System call
extern "C" int vfork_start( void (*long_jump)( jmp_buf*, int ), jmp_buf* buffer );


static void kerosene_long_jump( jmp_buf* buffer, int second_result )
{
	vfork_pop();
	
	longjmp( *buffer, second_result );
}

int _vfork_start( jmp_buf* buffer )
{
	int result = vfork_start( kerosene_long_jump, buffer );
	
	if ( result == 0 )
	{
		vfork_push();
	}
	
	return result;
}

