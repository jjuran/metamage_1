/*	========
 *	vfork.cc
 *	========
 */

#include "vfork.h"

// Kerosene
#include "environ.hh"


#pragma exceptions off


// System call
extern "C" int vfork_start( void (*long_jump)( sigjmp_buf*, int ), sigjmp_buf* buffer );


static void kerosene_long_jump( sigjmp_buf* buffer, int second_result )
{
	_pop_environ();
	
	siglongjmp( *buffer, second_result );
}

int _vfork_start( sigjmp_buf* buffer )
{
	int result = vfork_start( kerosene_long_jump, buffer );
	
	if ( result == 0 )
	{
		_push_environ();
	}
	
	return result;
}

