/*	=======
 *	idle.cc
 *	=======
 */

// POSIX
#include <unistd.h>


#pragma exceptions off


int main()
{
	pause();  // Never returns; process dies on signal
	
	// Not reached
	
	return 0;
}

