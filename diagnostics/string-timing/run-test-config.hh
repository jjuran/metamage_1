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
	
	clock_t best = 0;
	
	for ( int trial = 0;  trial < n_trials;  ++trial )
	{
		timer t( test_names[ I ] );
		
		for ( int j = 0;  j < n;  ++j )
		{
			#include "tests.hh"
		}
		
		const clock_t result = t.get();
		
		if ( best == 0  ||  result < best )
		{
			best = result;
		}
	}
	
	printf( "  %4d", best / K );
}
catch ( const skipped_test& )
{
	printf( "  %s", "SKIP" );
}

fflush( stdout );

