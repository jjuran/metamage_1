/*
	kill-systemstats.cc
	-------------------
*/

// Standard C
#include <stdlib.h>


int main( int argc, char** argv )
{
	system( "/usr/bin/sudo /usr/bin/killall -9 systemstats" );
	
	return 0;
}
