/*
	kill-mds.cc
	-----------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>


int main( int argc, char** argv )
{
	setuid( 0 );  // set real-user-id for killall
	
	system( "/usr/bin/killall -9 mds" );
	
	return 0;
}
