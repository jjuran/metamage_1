/*
	abort.cc
	--------
*/

// Standard C
#include <signal.h>
#include <stdlib.h>
#include <string.h>


#pragma exceptions off


static void sigusr1( int )
{
	abort();
}

int main( int argc, char const *const argv[] )
{
	if ( argc > 1  &&  strcmp( argv[1], "signal" ) == 0 )
	{
		signal( SIGUSR1, &sigusr1 );
		
		raise( SIGUSR1 );
	}
	
	abort();
	
	return 1;
}
