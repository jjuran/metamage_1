/*	========
 *	abort.cc
 *	========
 */

// Standard C
#include <stdlib.h>


#pragma exceptions off


#pragma export on

int main( int argc, char const *const argv[] )
{
	abort();
	
	return 1;
}

#pragma export reset

