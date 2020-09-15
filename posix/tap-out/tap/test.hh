/*
	test.hh
	
	Copyright 2009, Joshua Juran
*/

#ifndef TAPOUT_TEST_HH
#define TAPOUT_TEST_HH

// iota
#include "iota/iterator.hh"
#include "iota/strings.hh"

// plus
#include "plus/printable.hh"

// tap-out
#include "tap/log.hh"


#define STR_( x ) #x
#define STR( x ) STR_( x )

#define LINEREF()  __FILE__ ":" STR(__LINE__) ": test failed:\n"

#define EXPECT( cond )  ::tap::expect( cond, STR_LEN( LINEREF() ) )

#define EXPECT_NULL( p )  EXPECT( (p) == 0 )  // NULL

#define EXPECT_CMP( p, p_len, q, q_len )  EXPECT( ::tap::cmp( (p), (p_len), (q), (q_len) ) == 0 )

#define EXPECT_EQ( a, b )  EXPECT( ::tap::eq( (a), (b) ) )


namespace tap
{
	
	void start( const char* program_name, unsigned tests_planned );
	
	
	void print( bool ok );
	
	void expect( bool condition, const char* ref, unsigned len );
	
	int cmp( const void* p, unsigned p_len, const void* q, unsigned q_len );
	
	template < class A, class B >
	bool eq( const A& a, const B& b )
	{
		const bool equal = a == b;
		
		if ( !equal )
		{
			using iota::make_span;
			using plus::printable;
			
			log_expected( make_span( printable( b ) ) );
			log_received( make_span( printable( a ) ) );
		}
		
		return equal;
	}
	
}

#endif
