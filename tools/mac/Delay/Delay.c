/*
	Delay.c
	-------
*/

// Mac OS
#include <OSUtils.h>

// Standard C
#include <stdio.h>
#include <stdlib.h>


int main( int argc, char** argv )
{
	if ( argv[1] )
	{
		UInt32 current;
		
		Delay( atoi( argv[1] ), &current );
		
		printf( "%d\n", current );
	}
	
	return 0;
}

