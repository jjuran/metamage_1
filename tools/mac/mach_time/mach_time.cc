/*
	mach_time.cc
	------------
*/

// Mach
#ifdef __MACH__
#include <mach/mach_time.h>
#endif

// Standard C
#include <stdio.h>


static
const mach_timebase_info_data_t& get_timebase_info()
{
	static mach_timebase_info_data_t timebase_info;
	
	if ( timebase_info.denom == 0 )
	{
		mach_timebase_info( &timebase_info );
		
		while ( ((timebase_info.numer | timebase_info.denom) & 1) == 0 )
		{
			timebase_info.numer >>= 1;
			timebase_info.denom >>= 1;
		}
	}
	
	return timebase_info;
}

int main( int argc, char** argv )
{
	const int n = 8;
	
	uint64_t t[ n ];
	
	for ( int i = 0;  i < n;  ++i )
	{
		t[ i ] = mach_absolute_time();
	}
	
	for ( int i = 0;  i < n;  ++i )
	{
		printf( "mach_absolute_time:  %llu\n", (long long) t[ i ] );
	}
	
	mach_timebase_info_data_t timebase_info = get_timebase_info();
	
	printf( "timebase_info.numer: %lu\n", (long) timebase_info.numer );
	printf( "timebase_info.denom: %lu\n", (long) timebase_info.denom );
	
	const uint64_t max_ULL = 18446744073709551615ull;
	const uint64_t max_now = max_ULL / timebase_info.numer;
	
	uint64_t now = mach_absolute_time();
	
	printf( "mach_absolute_time:  %llu\n", (long long) now );
	
	int b = 0;
	
	for ( ;  now > max_now;  ++b )
	{
		now >>= 1;
	}
	
	now *= timebase_info.numer;
	now /= timebase_info.denom;
	
	now <<= b;
	
	printf( "nanoseconds:         %llu\n", (long long) now );
	
	now /= 1000;
	
	printf( "microseconds:        %llu\n", (long long) now );
	
	now /= 1000;
	
	printf( "milliseconds:        %llu\n", (long long) now );
	
	now /= 1000;
	
	printf( "seconds:             %llu\n", (long long) now );
	
	now /= 60;
	
	printf( "minutes:             %llu\n", (long long) now );
	
	now /= 60;
	
	printf( "hours:               %llu\n", (long long) now );
	
	now /= 24;
	
	printf( "days:                %llu\n", (long long) now );
	
	return 0;
}
