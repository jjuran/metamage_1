/*
	SysBeep.c
	---------
*/

// Mac OS
#include <Sound.h>

// Standard C
#include <stdlib.h>


int main( int argc, char** argv )
{
	int ticks = argv[1] ? atoi( argv[1] )
	                    : 30;
	
	SysBeep( ticks );
	
	return 0;
}

