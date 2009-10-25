/*
	check.cc
	
	Copyright 2009, Joshua Juran
*/

#include "tap/check.hh"

// Standard C
#include <stdlib.h>

// more-posix
#include "more/perror.hh"


namespace tap
{
	
	static const char* global_program_name = "tap-out";
	
	
	void set_program_name( const char* name )
	{
		global_program_name = name;
	}
	
	
	int check( int result, const char* text )
	{
		if ( result < 0 )
		{
			more::perror( global_program_name, text );
			
			exit( EXIT_FAILURE );
		}
		
		return result;
	}
	
}

