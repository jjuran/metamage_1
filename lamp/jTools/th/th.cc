/*	=====
 *	th.cc
 *	=====
 */

// Standard C
#include <errno.h>
#include <stdlib.h>

// Standard C/C++
#include <cctype>

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// POSIX
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vfork.h>

// Nucleus
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Io
#include "Io/TextInput.hh"

// Orion
#include "Orion/Main.hh"


namespace NN = Nucleus;
namespace p7 = poseven;
namespace O = Orion;


#define STR_LEN( str )  "" str, (sizeof str - 1)


static int exit_from_wait( int stat )
{
	int result = WIFEXITED( stat )   ? WEXITSTATUS( stat )
	           : WIFSIGNALED( stat ) ? WTERMSIG( stat ) + 128
	           :                       -1;
	
	return result;
}


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
	
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		int closed = close( pipe_ends[0] );
		
		int duped = dup2( pipe_ends[1], STDOUT_FILENO );
		
		closed = close( pipe_ends[1] );
		
		const char* argv[] = { "", NULL };
		
		argv[0] = test_file;
		
		execv( test_file, (char**) argv );
		
		_exit( 127 );
	}
	
	int closed = close( pipe_ends[1] );
	
	int wait_status = -1;
	pid_t resultpid = waitpid( pid, &wait_status, 0 );
	
	Io::TextInputAdapter< NN::Owned< p7::fd_t > > input( NN::Owned< p7::fd_t >::Seize( p7::fd_t( pipe_ends[0] ) ) );
	
	std::string plan = input.Read();
	
	TestResults results;
	
	if ( plan.substr( 0, 3 ) != "1.." )
	{
		return results;
	}
	
	results.planned = std::atoi( plan.c_str() + 3 );
	
	unsigned next_test_number = 1;
	
	while ( !input.Ended() )
	{
		std::string line = input.Read();
		
		if ( line[0] == '#' )
		{
			continue;
		}
		
		const char* comment = std::strchr( line.c_str(), '#' );
		
		bool todo = comment != NULL  &&  std::memcmp( comment, STR_LEN( "# TODO" ) ) == 0;
		
		const char* number = NULL;
		
		bool passed = false;
		
		if ( line.substr( 0, 3 ) == "ok " )
		{
			passed = true;
			number = line.c_str() + 3;
		}
		else if ( line.substr( 0, 7 ) == "not ok " )
		{
			number = line.c_str() + 7;
			
			if ( !todo  &&  results.failure == 0 )
			{
				results.failure = next_test_number;
			}
		}
		else
		{
			return results;
		}
		
		if ( std::atoi( number ) != next_test_number++ )
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

int O::Main( int argc, argv_t argv )
{
	const char* const* test_file = argv;
	
	TestResults totals;
	
	while ( *++test_file != NULL )
	{
		const char* test_name = *test_file;
		
		if ( test_name[0] == 't'  &&  test_name[1] == '/' )
		{
			test_name += 2;
		}
		
		unsigned name_length = std::strlen( test_name );
		
		if ( test_name[ name_length - 1 ] == 't'  &&  test_name[ name_length - 2 ] == '.' )
		{
			name_length -= 2;
		}
		
		unsigned width = std::max( 18U, name_length );
		
		write( STDOUT_FILENO, test_name,                    name_length );
		write( STDOUT_FILENO, "..................", width - name_length );
		
		TestResults results = run_test( *test_file );
		
		std::string result = "ok";
		
		if ( results.failure == 0 )
		{
			if ( results.unexpected > 0 )
			{
				result += ", with " + NN::Convert< std::string >( results.unexpected ) + " tests unexpectedly passing";
			}
			
			if ( results.todo > 0 )
			{
				result += ", but " + NN::Convert< std::string >( results.todo ) + " tests TODO failed";
			}
		}
		else
		{
			result = "FAILED at test " + NN::Convert< std::string >( results.failure );
		}
		
		result += "\n";
		
		write( STDOUT_FILENO, result.data(), result.size() );
		
		totals = totals + results;
	}
	
	Report( totals.planned,    "total"                  );
	Report( totals.passed,     "passed"                 );
	Report( totals.failed,     "failed"                 );
	Report( totals.todo,       "TODO failed"            );
	Report( totals.unexpected, "UNEXPECTEDLY SUCCEEDED" );
	
	return 0;
}

