/*	========
 *	mread.cc
 *	========
 */

// Standard C++
#include <algorithm>

// POSIX
#include <unistd.h>

// must
#include "must/write.h"

// Iota
#include "iota/strings.hh"


static const std::size_t global_buffer_length = 4096;

static char global_buffer[ global_buffer_length ];

static char* global_begin = global_buffer;
static char* global_mark  = global_buffer;
static char* global_end   = global_buffer + global_buffer_length;

static const char* get_buffer()
{
	return global_begin;
}

static const char* get_mark()
{
	return global_mark;
}

static char* get_next_cr()
{
	char* cr = std::find( global_begin, global_mark, '\r' );
	
	// CR must not be last
	while ( cr >= global_mark - 1 )
	{
		int bytes_read = read( STDIN_FILENO, global_mark, global_end - global_mark );
		
		if ( bytes_read <= 0 )
		{
			std::abort();
		}
		
		global_mark += bytes_read;
		
		cr = std::find( global_begin, global_mark, '\r' );
	}
	
	return cr;
}

static void advance_buffer( char* new_start )
{
	global_begin = new_start;
}

static bool equal_strings( const char* begin1,
                           const char* end1,
                           const char* begin2,
                           std::size_t len2 )
{
	return end1 - begin1 == len2  &&  std::equal( begin1, end1, begin2 );
}


int main( int argc, const char *const argv[] )
{
	while ( true )
	{
		char const* buffer  = get_buffer();
		char      * newline = get_next_cr();
		
		bool hasLF = newline[ 1 ] == '\n';
		
		if ( hasLF )
		{
			if ( equal_strings( buffer, newline, STR_LEN( "OK" ) ) )
			{
				return 0;
			}
			
			*newline = '\n';
			
			must_write( STDOUT_FILENO, buffer, newline + 1 - buffer );
			
			if ( equal_strings( buffer, newline, STR_LEN( "ERROR" ) ) )
			{
				return 1;
			}
			
			if ( equal_strings( buffer, newline, STR_LEN( "BUSY"         ) ) )  break;
			if ( equal_strings( buffer, newline, STR_LEN( "NO ANSWER"    ) ) )  break;
			if ( equal_strings( buffer, newline, STR_LEN( "NO CARRIER"   ) ) )  break;
			if ( equal_strings( buffer, newline, STR_LEN( "NO DIAL TONE" ) ) )  break;
		}
		
		advance_buffer( newline + 1 + hasLF );
	}
	
	return 2;  // Status message is last line of output
}

