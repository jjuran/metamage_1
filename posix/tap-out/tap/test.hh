/*
	test.hh
	
	Copyright 2009, Joshua Juran
*/

#ifndef TAPOUT_TEST_HH
#define TAPOUT_TEST_HH

// Standard C/C++
#include <cstring>

// iota
#include "iota/strings.hh"


#define STR_( x ) #x
#define STR( x ) STR_( x )

#define LINEREF()  __FILE__ ":" STR(__LINE__) ": test failed:\n"

#define EXPECT( cond )  ::tap::expect( cond, STR_LEN( LINEREF() ) )

#define EXPECT_NULL( p )  EXPECT( (p) == NULL )


namespace tap
{
	
	void start( const char* program_name, unsigned tests_planned );
	
	
	void print( bool ok, const char* comment = NULL );
	
	void expect( bool condition, const char* ref, unsigned len );
	
	inline void ok_if( bool ok, const char* comment = NULL )
	{
		print( ok, comment );
	}
	
	inline void ok_if_null( const void* p, const char* comment = NULL )
	{
		print( p == NULL, comment );
	}
	
	inline void ok_if_nonnull( const void* p, const char* comment = NULL )
	{
		print( p != NULL, comment );
	}
	
	inline void ok_if_strings_equal( const char* a,
	                                 const char* b,
	                                 const char* comment = NULL )
	{
		const bool ok = a != NULL  &&  b != NULL  &&  std::strcmp( a, b ) == 0;
		
		print( ok, comment );
	}
	
}

#endif
