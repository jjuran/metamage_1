/*
	gettimeofday.cc
	---------------
*/

// POSIX
#include <sys/time.h>

// Standard C
#include <stdio.h>


int main()
{
	const int n = 8;
	
	timeval t[ n ];
	
	for ( int i = 0;  i < n;  ++i )
	{
		int got = gettimeofday( &t[ i ], NULL );
		
		if ( got < 0 )
		{
			return 1;  // shouldn't happen
		}
	}
	
	for ( int i = 0;  i < n;  ++i )
	{
		const timeval& tv = t[ i ];
		
		printf( "%ld.%.6ld\n", (long) tv.tv_sec, (long) tv.tv_usec );
	}
	
	return 0;
}
