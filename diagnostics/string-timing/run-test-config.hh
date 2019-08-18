/*
	run-test-config.hh
	------------------
*/


try
{
	for ( int j = 0;  j < 100;  ++j )
	{
		#include "tests.hh"
	}
	
	unsigned long long best = 0;
	
	for ( int trial = 0;  trial < n_trials;  ++trial )
	{
		timer t( test_names[ I ] );
		
		for ( int j = 0;  j < n;  ++j )
		{
			#include "tests.hh"
		}
		
		const unsigned long long result = t.get();
		
		if ( best == 0  ||  result < best )
		{
			best = result;
		}
	}
	
	printf( "  %4llu", best / K );
}
catch ( const skipped_test& )
{
	printf( "  %s", "SKIP" );
}

fflush( stdout );
