/*	=====
 *	th.cc
 *	=====
 */

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// Standard C/C++
#include <cctype>
#include <cstdio>
#include <cstring>

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// Iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/close.hh"
#include "poseven/functions/execv.hh"
#include "poseven/functions/dup2.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/wait.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	struct TestResults
	{
		int planned;
		int failure;
		int passed;
		int failed;
		int todo;
		int unexpected;
		
		TestResults() : planned(),
		                failure(),
		                passed(),
		                failed(),
		                todo(),
		                unexpected()
		{
		}
	};
	
	static TestResults operator+( const TestResults& a, const TestResults& b )
	{
		TestResults sum;
		
		sum.planned    = a.planned    + b.planned;
		sum.passed     = a.passed     + b.passed;
		sum.failed     = a.failed     + b.failed;
		sum.todo       = a.todo       + b.todo;
		sum.unexpected = a.unexpected + b.unexpected;
		
		return sum;
	}
	
	static TestResults run_test( const char* test_file )
	{
		int pipe_ends[2];
		
		int piped = pipe( pipe_ends );
		
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			int closed = close( pipe_ends[0] );
			
			p7::dup2( p7::fd_t( pipe_ends[1] ), p7::stdout_fileno );
			
			closed = close( pipe_ends[1] );
			
			const char* argv[] = { "", NULL };
			
			argv[0] = test_file;
			
			p7::execv( argv );
		}
		
		int closed = close( pipe_ends[1] );
		
		(void) p7::wait();
		
		TestResults results;
		
		text_input::feed feed;
		
		n::owned< p7::fd_t > input_fd( n::owned< p7::fd_t >::seize( p7::fd_t( pipe_ends[0] ) ) );
		
		p7::fd_reader reader( input_fd );
		
		const plus::string* plan = get_line_from_feed( feed, reader );
		
		if ( plan == NULL  ||  memcmp( plan->c_str(), STR_LEN( "1.." ) + 1 ) != 0 )
		{
			return results;
		}
		
		results.planned = iota::parse_unsigned_decimal( plan->c_str() + 3 );
		
		unsigned next_test_number = 1;
		
		while ( const plus::string* s = get_line_from_feed( feed, reader ) )
		{
			const char* begin = s->c_str();
			
			if ( begin[ 0 ] == '#' )
			{
				continue;
			}
			
			const char* comment = std::strchr( begin, '#' );
			
			const bool todo = comment  &&  memcmp( comment,
			                                       STR_LEN( "# TODO" ) ) == 0;
			
			const char* number = NULL;
			
			bool passed = false;
			
			if ( memcmp( begin, STR_LEN( "ok " ) ) == 0 )
			{
				passed = true;
				
				number = begin + STRLEN( "ok " );
			}
			else if ( memcmp( begin, STR_LEN( "not ok " ) ) == 0 )
			{
				number = begin + STRLEN( "not ok " );
				
				if ( !todo  &&  results.failure == 0 )
				{
					results.failure = next_test_number;
				}
			}
			else
			{
				return results;
			}
			
			if ( iota::parse_unsigned_decimal( number ) != next_test_number++ )
			{
				return results;
			}
			
			int& result = passed ? todo ? results.unexpected
			                            : results.passed
			                     : todo ? results.todo
			                            : results.failed;
			
			++result;
		}
		
		return results;
	}
	
	static void Report( unsigned count, const char* status )
	{
		if ( count > 0 )
		{
			std::printf( "%d test%s %s\n", count, count == 1 ? "" : "s", status );
		}
	}
	
	int Main( int argc, char** argv )
	{
		const char* const* test_files = argv;
		
		TestResults totals;
		
		while ( *++test_files != NULL )
		{
			const char* test_file = *test_files;
			
			struct stat sb;
			
			if ( -1 == ::stat( test_file, &sb )  ||  S_ISDIR( sb.st_mode ) && (errno = EISDIR) )
			{
				std::fprintf( stderr, "th: %s: %s\n", test_file, std::strerror( errno ) );
				
				return EXIT_FAILURE;
			}
			
			const char* test_name = test_file;
			
			if ( test_name[0] == 't'  &&  test_name[1] == '/' )
			{
				test_name += 2;
			}
			
			unsigned name_length = std::strlen( test_name );
			
			if ( test_name[ name_length - 1 ] == 't'  &&  test_name[ name_length - 2 ] == '.' )
			{
				name_length -= 2;
			}
			
			unsigned width = std::max( 32U, name_length );
			
			write( STDOUT_FILENO, test_name,                                    name_length );
			write( STDOUT_FILENO, "..................................", width - name_length );
			
			TestResults results = run_test( test_file );
			
			plus::var_string result = "ok";
			
			if ( results.failure == 0 )
			{
				if ( results.unexpected > 0 )
				{
					result += ", with ";
					result += iota::inscribe_decimal( results.unexpected );
					result += " tests unexpectedly passing";
				}
				
				if ( results.todo > 0 )
				{
					result += ", but ";
					result += iota::inscribe_decimal( results.todo );
					result += " tests TODO failed";
				}
			}
			else
			{
				result = "FAILED at test ";
				result += iota::inscribe_decimal( results.failure );
			}
			
			result += "\n";
			
			p7::write( p7::stdout_fileno, result );
			
			totals = totals + results;
		}
		
		Report( totals.planned,    "total"                  );
		Report( totals.passed,     "passed"                 );
		Report( totals.failed,     "failed"                 );
		Report( totals.todo,       "TODO failed"            );
		Report( totals.unexpected, "UNEXPECTEDLY SUCCEEDED" );
		
		return 0;
	}

}

