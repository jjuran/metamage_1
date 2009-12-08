/*	===============
 *	debug/assert.cc
 *	===============
 */

#include "debug/assert.hh"

// Standard C
#include <stdlib.h>

// POSIX
#include <unistd.h>
#include <sys/uio.h>

// Iota
#include "iota/decimal.hh"
#include "iota/strings.hh"


namespace debug
{
	
	void handle_failed_assertion( const char*  text,
	                              const char*  func,
	                              const char*  file,
	                              unsigned     line )
	{
		const unsigned magnitude = iota::decimal_magnitude( line );
		
		char line_buffer[ sizeof "1234567890" ];
		
		iota::fill_unsigned_decimal( line, line_buffer, magnitude );
		
		struct iovec iov[] =
		{
			{ (void*) STR_LEN( "ASSERT( "  ) },
			{ (void*) text, strlen( text ) },
			{ (void*) STR_LEN( ") in " ) },
			{ (void*) func, strlen( func ) },
			{ (void*) STR_LEN( "(); " ) },
			{ (void*) file, strlen( file ) },
			{ (void*) STR_LEN( ", line "      ) },
			{ (void*) line_buffer, magnitude },
			{ (void*) STR_LEN( "\n"           ) },
		};
		
		ssize_t written = writev( STDERR_FILENO, iov, sizeof iov / sizeof iov[0] );
		
		abort();
	}
	
}

