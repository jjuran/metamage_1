/*
	check.hh
	
	Copyright 2009, Joshua Juran
*/

#ifndef TAPOUT_CHECK_HH
#define TAPOUT_CHECK_HH


#define CHECK( call )  ::tap::check( call, #call )


namespace tap
{
	
	void set_program_name( const char* name );
	
	int check( int result, const char* text );
	
}

#endif

