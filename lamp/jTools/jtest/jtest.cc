/*	========
 *	jtest.cc
 *	========
 */

// Standard C
#include "errno.h"
#include "stdlib.h"

// Standard C/C++
#include <cctype>

// Standard C++
#include <algorithm>
#include <functional>

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


static bool PathnameMeansStdIn( const char* pathname )
{
	return    pathname[0] == '-'
	       && pathname[1] == '\0';
}

static int exit_from_wait( int stat )
{
	int result = WIFEXITED( stat )   ? WEXITSTATUS( stat )
	           : WIFSIGNALED( stat ) ? WTERMSIG( stat ) + 128
	           :                       -1;
	
	return result;
}

enum IoOperation
{
	kNoOp,               // 0  nil
	kOutputMatchesThis,  // 1  >=
	kOutputMatchesHere,  // 2  >>
	kInputThis,          // 3  <=
	kInputHere,          // 4  <<
	kClosed              // 5  --
};

static IoOperation ReadOp( const char* s )
{
	bool input = false;
	bool here = false;
	
	switch ( s[0] )
	{
		case '-':
			return s[1] == '-' ? kClosed : kNoOp;
		
		case '<':
			input = true;
			// fall through
		
		case '>':
			if      ( s[1] == s[0] )  here = true;
			else if ( s[1] != '='  )  return kNoOp;
			
			return IoOperation( (input << 1 | here) + 1 );
		
		default:
			return kNoOp;
	}
}

struct Redirection
{
	int          fd;
	IoOperation  op;
	std::string  data;
	
	Redirection( int fd, IoOperation op, const std::string& data ) : fd( fd ), op( op ), data( data )  {}
};

static std::vector< Redirection > gRedirections;

static unsigned gPipesNeeded = 0;

struct Pipe
{
	int itsFDs[2];
	
	Pipe()  {}
	
	Pipe( const int fds[2] )  { std::copy( fds, fds + 2, itsFDs ); }
};

static std::vector< Pipe > gPipes;

static void AddRedirection( int fd, IoOperation op, const std::string& param )
{
	gRedirections.push_back( Redirection( fd, op, param ) );
	
	if ( op == kOutputMatchesThis )
	{
		++gPipesNeeded;
	}
}

static void ApplyRedirection( Redirection& redir )
{
	int pipe_fds[2];
	int* fds = pipe_fds;
	
	switch ( redir.op )
	{
		case kClosed:
			close( redir.fd );
			break;
		
		case kInputThis:
			pipe( fds );
			write( fds[1], redir.data.data(), redir.data.size() );
			close( fds[1] );
			dup2( fds[0], redir.fd );
			close( fds[0] );
			break;
		
		case kOutputMatchesThis:
			fds = gPipes.back().itsFDs;
			dup2( fds[1], redir.fd );
			close( fds[1] );
			redir.fd = fds[0];
			gPipes.pop_back();
			break;
		
		default:
			std::fprintf( stderr, "Error in redirection\n" );
			break;
	}
}

static bool DiscrepantOutput( const Redirection& redir )
{
	if ( redir.op != kOutputMatchesThis )  return false;
	
	char data[ 4096 ];
	
	int bytes_read = read( redir.fd, data, 4096 -1 );
	
	if ( bytes_read == -1 )
	{
		bytes_read = 0;
		std::perror( "Error reading captured output" );
	}
	
	data[ bytes_read ] = '\0';
	
	close( redir.fd );
	
	bool match = bytes_read == redir.data.size()  &&  std::memcmp( data, redir.data.data(), bytes_read ) == 0;
	
	if ( !match )
	{
		std::fprintf( stderr, "EXPECTED: '%s'\n" "RECEIVED: '%s'\n", redir.data.c_str(), data );
	}
	
	return !match;
}

int O::Main( int argc, const char *const argv[] )
{
	const char* jtest = argv[0];
	
	unsigned number = 0;
	
	if ( argc > 2  &&  std::string( argv[1] ) == "--num" )
	{
		number = std::atoi( argv[2] );
		
		argc -= 2;
		argv += 2;
	}
	
	int fd = 0;  // Default to stdin
	
	if ( argc > 1  &&  !PathnameMeansStdIn( argv[1] ) )
	{
		fd = open( argv[1], O_RDONLY );
		
		if ( fd == -1 )
		{
			std::fprintf( stderr, "%s: %s: %s\n", jtest, argv[1], std::strerror( errno ) );
			
			return 1;
		}
	}
	
	Io::TextInputAdapter< P7::FileDescriptor > input = P7::FileDescriptor( fd );
	
	std::string command;
	int exit_status = 0;
	
	while ( !input.Ended() )
	{
		std::string line = input.Read();
		
		if ( line.empty() )  continue;
		
		if ( line[0] == '#' )
		{
			//number = std::atoi( line.substr( line.find_first_not_of( " \t", 1 ), line.npos ).c_str() );
			continue;
		}
		
		if ( line[0] == '$' )
		{
			command = line.substr( line.find_first_not_of( " \t", 1 ), line.npos );
			continue;
		}
		
		if ( line[0] == '?' )
		{
			exit_status = std::atoi( line.substr( line.find_first_not_of( " \t", 1 ), line.npos ).c_str() );
			continue;
		}
		
		if ( std::isdigit( line[0] ) )
		{
			std::size_t end_of_fd = line.find_first_not_of( "0123456789" );
			
			int fd_to_redirect = std::atoi( line.substr( 0, end_of_fd - 0 ).c_str() );
			
			std::size_t start_of_op = line.find_first_not_of( " \t", end_of_fd );
			
			std::size_t end_of_op = start_of_op + 2;
			
			if ( line.size() < end_of_op )
			{
				std::fprintf( stderr, "Missing operator in line: %s\n", line.c_str() );
				return 1;
			}
			
			IoOperation op = ReadOp( line.c_str() + start_of_op );
			
			if ( op == kNoOp )
			{
				std::fprintf( stderr, "Unrecognized operator in line: %s\n", line.c_str() );
				return 1;
			}
			
			std::string param;
			
			if ( op != kClosed )
			{
				std::size_t start_of_param = line.find_first_not_of( " \t", end_of_op );
				
				char param0 = line[ start_of_param ];
				char delimiter = ' ';
				
				if ( param0 == '"'  ||  param0 == '\'' )
				{
					delimiter = param0;
					
					++start_of_param;
				}
				
				std::size_t end_of_param = line.find( delimiter, start_of_param );
				
				param = line.substr( start_of_param, end_of_param - start_of_param );
				
				param += "\n";
			}
			
			AddRedirection( fd_to_redirect, op, param );
			
			continue;
		}
		
		std::fprintf( stderr, "Unprocessed line: %s\n", line.c_str() );
	}
	
	if ( command.empty() )
	{
		std::fprintf( stderr, "Command missing\n" );
		return 1;
	}
	
	while ( gPipesNeeded-- )
	{
		int fds[2];
		pipe( fds );
		gPipes.push_back( fds );
	}
	
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		std::for_each( gRedirections.begin(),
		               gRedirections.end(),
		               std::ptr_fun( ApplyRedirection ) );
		
		const char* argv[] = { "sh", "-c", "", NULL };
		
		argv[2] = command.c_str();
		
		execv( "/bin/sh", argv );
		
		_exit( 127 );
	}
	
	int wait_status = -1;
	pid_t resultpid = waitpid( pid, &wait_status, 0 );
	
	std::vector< Redirection >::const_iterator it = std::find_if( gRedirections.begin(),
	                                                              gRedirections.end(),
	                                                              std::ptr_fun( DiscrepantOutput ) );
	
	bool output_matches = it == gRedirections.end();
	bool status_matches = exit_from_wait( wait_status ) == exit_status;
	
	bool test = status_matches && output_matches;
	
	std::string result = test ? "ok" : "not ok";
	
	if ( number > 0 )
	{
		result += " " + NN::Convert< std::string >( number );
	}
	
	result += "\n";
	
	(void) write( STDOUT_FILENO, result.data(), result.size() );
	
	return 0;
}

