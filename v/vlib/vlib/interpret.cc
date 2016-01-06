/*
	interpret.cc
	------------
*/

#include "vlib/interpret.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// Standard C++
#include <new>

// must
#include "must/write.h"

// vlib
#include "vlib/calc.hh"
#include "vlib/parse.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  ::vlib::fail( STR_LEN( "ERROR: " s "\n" ) )


namespace vlib
{
	
	static
	void fail( const char* msg, unsigned len )
	{
		must_write( STDERR_FILENO, msg, len );
	
		exit( 1 );
	}
	
	Value interpret( const char* program, const char* file )
	{
		try
		{
			return eval_tree( parse( program, file ) );
		}
		catch ( const std::bad_alloc& )
		{
			FAIL( "Out of memory!" );
		}
		catch ( const plus::ibox::limb_count_overflow& )
		{
			FAIL( "Max bigint size exceeded" );
		}
		catch ( const transfer_via_break& )
		{
			FAIL( "`break` used outside of loop" );
		}
		catch ( const transfer_via_continue& )
		{
			FAIL( "`continue` used outside of loop" );
		}
		
		return Value();
	}
	
}
