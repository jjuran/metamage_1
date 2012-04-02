/*
	run-test.hh
	-----------
*/

#undef STRING
#undef VAR_STRING
#undef PLUS_STRING

{
	i = I;
	
	printf( "%2d  %s:", i, test_names[ i ] );
	
	fflush( stdout );
	
	#define STRING      std::string
	#define VAR_STRING  std::string
	
	#include "run-test-config.hh"
	
	#undef STRING
	#undef VAR_STRING
	#define STRING      plus::string
	#define VAR_STRING  plus::cow_string
	#define PLUS_STRING 1
	
	#include "run-test-config.hh"
	
	#undef VAR_STRING
	#define VAR_STRING  plus::var_string
	
	#include "run-test-config.hh"
	
	printf( "\n" );
}

