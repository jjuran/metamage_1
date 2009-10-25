/*
	test.hh
	
	Copyright 2009, Joshua Juran
*/

#ifndef TAPOUT_TEST_HH
#define TAPOUT_TEST_HH

// Standard C/C++
#include <cstring>


namespace tap
{
	
	void start( const char* program_name, unsigned tests_planned );
	
	
	void ok_if( bool ok, const char* comment = NULL );
	
	inline void ok_if_null( const void* p, const char* comment = NULL )
	{
		ok_if( p == NULL, comment );
	}
	
	inline void ok_if_nonnull( const void* p, const char* comment = NULL )
	{
		ok_if( p != NULL, comment );
	}
	
	inline void ok_if_strings_equal( const char* a,
	                                 const char* b,
	                                 const char* comment = NULL )
	{
		const bool ok = a != NULL  &&  b != NULL  &&  std::strcmp( a, b ) == 0;
		
		ok_if( ok, comment );
	}
	
}

#endif

