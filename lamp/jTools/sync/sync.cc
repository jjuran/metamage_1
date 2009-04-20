/*	=======
 *	sync.cc
 *	=======
 */

// POSIX
#include <unistd.h>


#pragma exceptions off


int main( int argc, char const *const argv[] )
{
	sync();
	
	return 0;
}

