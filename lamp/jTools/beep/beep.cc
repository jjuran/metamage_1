/*	=======
 *	beep.cc
 *	=======
 */

// Mac OS
#ifndef __SOUND__
#include <Sound.h>
#endif


#pragma export on

int main( void )
{
	SysBeep( 30 );
	
	return 0;
}

#pragma export reset

