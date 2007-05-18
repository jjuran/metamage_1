/*	=====
 *	th.cc
 *	=====
 */

// Standard C
#include "errno.h"
#include "stdlib.h"

// Standard C/C++
#include <cctype>

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// POSIX
#include "fcntl.h"
#include "sys/wait.h"
#include "unistd.h"

// Nucleus
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Io
#include "Io/TextInput.hh"

// Orion
#include "Orion/Main.hh"


namespace NN = Nucleus;
namespace P7 = POSeven;
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
};

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
	
	Io::TextInputAdapter< NN::Owned< P7::FileDescriptor > > input( NN::Owned< P7::FileDescriptor >::Seize( P7::FileDescriptor( pipe_ends[0] ) ) );
	
	std::string plan = input.Read();
	
	TestResults results = { -1, 0, 0, 0, 0, 0 };
	
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

int O::Main( int argc, const char *const argv[] )
{
	const char* const* test_file = argv;
	
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
			
		}
		else
		{
			result = "FAILED at test " + NN::Convert< std::string >( results.failure );
		}
		
		result += "\n";
		
		write( STDOUT_FILENO, result.data(), result.size() );
	}
	
	
	return 0;
}

