/*	===============
 *	debug/assert.cc
 *	===============
 */

#include "debug/assert.hh"

// POSIX
#include <unistd.h>
#include <sys/uio.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// Iota
#include "iota/strings.hh"

// gear
#include "gear/inscribe_decimal.hh"


namespace debug
{
	
	void handle_failed_assertion( const char*  text,
	                              const char*  func,
	                              const char*  file,
	                              unsigned     line,
	                              bool         fatal )
	{
		const unsigned magnitude = gear::decimal_magnitude( line );
		
		char line_buffer[ sizeof "1234567890" ];
		
		gear::fill_unsigned_decimal( line, line_buffer, magnitude );
		
		const char* macro = fatal ? "ASSERT( "
		                          : "EXPECT( ";
		
		struct iovec iov[] =
		{
			{ (void*) macro, sizeof "ASSERT( " - 1 },
			{ (void*) text, strlen( text ) },
			{ (void*) STR_LEN( " ) in " ) },
			{ (void*) func, strlen( func ) },
			{ (void*) STR_LEN( "(); " ) },
			{ (void*) file, strlen( file ) },
			{ (void*) STR_LEN( ", line "      ) },
			{ (void*) line_buffer, magnitude },
			{ (void*) STR_LEN( "\n"           ) },
		};
		
		ssize_t written = writev( STDERR_FILENO, iov, sizeof iov / sizeof iov[0] );
		
		if ( fatal )
		{
			abort();
		}
	}
	
}
