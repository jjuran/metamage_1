// ============
// Execution.cc
// ============

#include "Execution.hh"

// Standard C/C++
#include <cctype>
#include <cstring>

// Standard C
#include "errno.h"
#include "stdlib.h"

// POSIX
#include "fcntl.h"
#include "sys/stat.h"
#include "sys/wait.h"
#include "unistd.h"

// POSeven
#include "POSeven/Errno.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// ShellShock
#include "ShellShock/Expansion.hh"
#include "ShellShock/StringArray.hh"

// sh
#include "Builtins.hh"
#include "Options.hh"
#include "PositionalParameters.hh"


namespace NN = Nucleus;
namespace P7 = POSeven;
namespace O = Orion;
namespace Sh = ShellShock;


using Sh::Command;
using Sh::Pipeline;
using Sh::Circuit;
using Sh::List;


int gLastResult = 0;


struct Job
{
	Circuit circuit;
	int pid;
};

typedef std::map< std::size_t, Job > JobTable;

static JobTable gJobTable;

static int NextJobNumber()
{
	return 0;
}

/*
static int NewJob( const Circuit& circuit )
{
	int job = NextJobNumber();
	
	gJobTable[ job ];
	
	return job;
	
	
	
}
*/


class AppendWithSpace
{
	public:
		AppendWithSpace( std::string& ioString ) : myString( ioString )  {}
		
		void operator()( const std::string& inNew )
		{
			myString += " " + inNew;
		}
	
	private:
		std::string& myString;
};


class ShellParameterDictionary : public Sh::ParameterDictionary
{
	public:
		ShellParameterDictionary()  {}
		
		std::string Lookup( const std::string& param ) const;
};

std::string ShellParameterDictionary::Lookup( const std::string& param ) const
{
	if ( param == "$" )
	{
		return NN::Convert< std::string >( getpid() );
	}
	else if ( param == "?" )
	{
		return NN::Convert< std::string >( gLastResult );
	}
	
	std::size_t paramCount = gParameterCount;
	
	int i;
	
	if ( param == "#" )
	{
		return NN::Convert< std::string >( paramCount );
	}
	else if ( param == "*" )
	{
		if ( paramCount == 0 )
		{
			return "";
		}
		
		std::string params = gParameters[ 0 ];
		
		std::for_each( gParameters + 1,
		               gParameters + gParameterCount,
		               AppendWithSpace( params ) );
		
		return params;
	}
	else if ( ( i = std::atoi( param.c_str() ) ) > 0 )
	{
		std::string value = ( i <= paramCount ) ? gParameters[ i - 1 ]
		                                        : std::string( "" );
		
		return value;
	}
	else if ( param == "0" )
	{
		return gArgZero;
		//return gParameters[ 0 ];
	}
	
	if ( const char* value = QueryShellVariable( param ) )
	{
		return value;
	}
	else if ( const char* var = getenv( param.c_str() ) )
	{
		return var;
	}
	
	return "";
}

static int Open( const char* path, mode_t mode )
{
	int opened = open( path, mode );
	
	P7::ThrowPOSIXResult( opened );
	
	return opened;
}

static int OpenNoClobber( const char* path )
{
	// Try a non-creative open first.
	int opened = open( path, O_WRONLY );
	
	if ( opened == -1 )
	{
		if ( errno != ENOENT )
		{
			P7::ThrowErrno( errno );
		}
		
		// No such file, exclusively create it, or bust.
		opened = Open( path, O_WRONLY | O_CREAT | O_EXCL );
		
		// Return exclusively created file.
		return opened;
	}
	
	struct ::stat sb;
	
	int status = fstat( opened, &sb );
	
	if ( status == -1  ||  S_ISREG( sb.st_mode ) )
	{
		// Either the stat failed and something's wrong, or a regular file exists.
		int error = status == -1 ? errno : EEXIST;
		
		// Close the file.
		int closed = close( opened );
		
		P7::ThrowErrno( error );
	}
	
	// Return non-regular file -- maybe a device/pipe/socket, etc.
	return opened;
}

/*
static int OpenNoClobber( const char* path )
{
	// Try exclusively creating first.
	int opened = open( path, O_WRONLY | O_CREAT | O_EXCL );
	
	if ( opened >= 0 )
	{
		// Return a newly created regular file.
		return opened;
	}
	
	if ( errno != EEXIST )
	{
		P7::ThrowErrno( errno );  // Some error occurred (besides an existing file)
	}
	
	// File exists.  Try again without creating.
	opened = Open( path, O_WRONLY );
	
	struct ::stat sb;
	
	int status = fstat( opened, &sb );
	
	if ( status == -1  ||  S_ISREG( sb.st_mode ) )
	{
		// Either the stat failed and something's wrong, or a regular file exists.
		int error = status == -1 ? errno : EEXIST;
		
		// Close the file.
		int closed = close( opened );
		
		P7::ThrowErrno( error );
	}
	
	// Return existing non-regular file -- maybe a device/pipe/socket, etc.
	return opened;
}
*/

static void Dup2( int oldfd, int newfd )
{
	P7::ThrowPOSIXResult( dup2( oldfd, newfd ) );
}

static void RedirectIO( Sh::Redirection redirection )
{
	int fd = redirection.fd;
	const char* param = redirection.param.c_str();
	
	int file = -1;
	
	try
	{
		switch ( redirection.op )
		{
			case Sh::kRedirectInput:
				file = Open( param, O_RDONLY );
				Dup2( file, fd );
				close( file );
				break;
			
			case Sh::kRedirectInputHere:
			case Sh::kRedirectInputHereStrippingTabs:
				break;
			
			case Sh::kRedirectInputDuplicate:
				file = std::atoi( param );  // FIXME:  Probably bad if param isn't an integer
				Dup2( file, fd );
				break;
			
			case Sh::kRedirectInputAndOutput:
				file = Open( param, O_RDWR );
				
				if ( fd == -1 )
				{
					Dup2( file, 0 );
					Dup2( file, 1 );
				}
				else
				{
					Dup2( file, fd );
				}
				
				close( file );
				break;
			
			case Sh::kRedirectOutput:
				if ( GetOption( kOptionNonClobberingRedirection ) )
				{
					file = OpenNoClobber( param );
					Dup2( file, fd );
					close( file );
					
					break;
				}
				// else fall through
				
			case Sh::kRedirectOutputClobbering:
				file = Open( param, O_WRONLY | O_CREAT | O_TRUNC );
				Dup2( file, fd );
				close( file );
				break;
			
			case Sh::kRedirectOutputAppending:
				file = Open( param, O_WRONLY | O_APPEND | O_CREAT );
				Dup2( file, fd );
				close( file );
				break;
			
			case Sh::kRedirectOutputDuplicate:
				file = std::atoi( param );  // FIXME:  Probably bad if atoi returns 0
				Dup2( file, fd );
				break;
			
			case Sh::kRedirectOutputAndError:
				file = Open( param, O_WRONLY | O_CREAT | O_TRUNC );
				Dup2( file, 1 );
				Dup2( file, 2 );
				close( file );
				break;
		}  // switch
	}
	catch ( const P7::Errno& errnum )
	{
		std::fprintf( stderr, "sh: %s: %s\n", param, strerror( errnum ) );
		
		throw;
	}
}

static void RedirectIOs( const std::vector< Sh::Redirection >& redirections )
{
	std::for_each( redirections.begin(),
				   redirections.end(),
				   std::ptr_fun( RedirectIO ) );
}

static int Exec( char const* const argv[] )
{
	const char* file = argv[ 0 ];
	
	int exec_result = execvp( file, const_cast< char** >( argv ) );
	
	const char* error_msg = errno == ENOENT ? "command not found" : std::strerror( errno );
	
	std::fprintf( stderr, "%s: %s: %s\n", "sh", file, error_msg );
	
	_exit( 127 );  // Use _exit() to exit a forked but not exec'ed process.
	
	return -1;
}

static int Wait( pid_t pid )
{
	int stat = -1;
	
	pid_t resultpid = waitpid( pid, &stat, 0 );
	
	if ( resultpid == -1 )
	{
		std::perror( "sh: waitpid() failed" );
	}
	
	return stat;
}

static int exit_from_wait( int stat )
{
	int result = WIFEXITED( stat )   ? WEXITSTATUS( stat )
	           : WIFSIGNALED( stat ) ? WTERMSIG( stat ) + 128
	           :                       -1;
	
	return result;
}


static int CallBuiltin( Builtin builtin, char** argv )
{
	int argc = Sh::CountStringArray( argv );
	
	return builtin( argc, argv );
}

static std::string EscapeForShell( const char* word )
{
	std::string result;
	
	for ( const char* p = word;  *p != '\0';  ++p )
	{
		if ( !std::isalnum( *p ) )
		{
			result += '\\';
		}
		
		result += *p;
	}
	
	return result;
}

static std::string MakeShellCommandFromBuiltin( char** argv )
{
	std::string command = *argv;
	
	while ( *++argv != NULL )
	{
		command += " ";
		command += EscapeForShell( *argv );
	}
	
	return command;
}

static bool CommandIsOnlyAssignments( char** argv )
{
	//ASSERT( argv != NULL );
	
	while ( *argv != NULL  &&  std::strchr( *argv, '=' ) )
	{
		++argv;
	}
	
	return *argv == NULL;
}

static int AssignShellVariablesFromArgV( char** argv )
{
	//ASSERT( argv != NULL );
	
	while ( *argv != NULL )
	{
		char* eq = std::strchr( *argv, '=' );
		
		//ASSERT( eq != NULL );
		
		const char* name = *argv++;
		const char* value = eq + 1;
		
		*eq = '\0';
		
		AssignShellVariable( name, value );
	}
	
	return 0;
}

static Command ParseCommand( const Command& command )
{
	return Sh::ParseCommand( command, ShellParameterDictionary() );
}

static int ExecuteCommand( const Command& command )
{
	Sh::StringArray argvec( command.args );
	
	char** argv = argvec.GetPointer();
	
	if ( argv[ 0 ] == NULL )
	{
		return 0;
	}
	
	if ( CommandIsOnlyAssignments( argv ) )
	{
		return AssignShellVariablesFromArgV( argv );
	}
	
	Builtin builtin = FindBuiltin( argv[ 0 ] );
	
	try
	{
		if ( builtin != NULL  &&  command.redirections.empty() )
		{
			return CallBuiltin( builtin, argv );
		}
		
		// This variable is set before and examined after a longjmp(), so it
		// needs to be volatile to make sure it doesn't wind up in a register
		// and subsequently clobbered.
		volatile bool exiting = false;
		
		int pid = vfork();
		
		if ( pid == -1 )
		{
			std::perror( "sh: vfork()" );
			
			return 127;
		}
		
		if ( pid == 0 )
		{
			if ( GetOption( kOptionMonitor ) )
			{
				setpgrp();
				
				tcsetpgrp( 0, getpgrp() );
			}
			
			try
			{
				RedirectIOs( command.redirections );
				
				if ( builtin != NULL )
				{
					try
					{
						// Since we didn't actually exec anything, we have to exit manually
						_exit( CallBuiltin( builtin, argv ) );
					}
					catch ( const O::ExitStatus& status )
					{
						// We get here if the 'exit' builtin was called.
						// But we can't let the exception unwind past vfork(),
						// so we catch it here, set a flag, and exit normally.
						exiting = true;
						
						_exit( status );
					}
					
					// Not reached
				}
				else
				{
					while ( std::strchr( argv[ 0 ], '=' ) )
					{
						putenv( argv[0] );
						
						++argv;
					}
					
					Exec( argv );
				}
				
				// Not reached
			}
			catch ( ... )
			{
				// Don't unwind past vfork().  That would be Bad.
			}
			
			// If the child hasn't exited by now, do so.
			_exit( 1 );
		}
		
		// Wait for the child process to exit
		int exit_status = exit_from_wait( Wait( pid ) );
		
		if ( exiting )
		{
			// The 'child' was the 'exit' builtin, meaning we should exit
			O::ThrowExitStatus( exit_status );
		}
		
		return exit_status;
	}
	catch ( const O::ExitStatus& )
	{
		// Rethrow.  We catch here so a thrown exit doesn't get caught below.
		throw;
	}
	catch ( ... )
	{
		Io::Err << "wish: An exception occurred while running the command.\n";
	}
	
	return 127;
}


static int ExecuteCommandFromPipeline( const Command& command )
{
	Sh::StringArray argvec( command.args );
	
	char** argv = argvec.GetPointer();
	
	if ( argv[ 0 ] == NULL )
	{
		return 0;
	}
	
	if ( CommandIsOnlyAssignments( argv ) )
	{
		// An assignment as the sole command to a shell is pointless, so just exit
		return 0;
	}
	
	try
	{
		RedirectIOs( command.redirections );
		
		while ( std::strchr( argv[ 0 ], '=' ) )
		{
			putenv( argv[0] );
			
			++argv;
		}
		
		if ( Builtin builtin = FindBuiltin( argv[ 0 ] ) )
		{
			std::string subshell = MakeShellCommandFromBuiltin( argv );
			
			const char* subshell_argv[] = { "/bin/sh", "-c", subshell.c_str(), NULL };
			
			return Exec( subshell_argv );
		}
		
		return Exec( argv );
		
	}
	catch ( const O::ExitStatus& status )
	{
		return status;
	}
	catch ( ... )
	{
		Io::Err << "wish: An exception occurred while running the command.\n";
	}
	
	return 127;
}

static void ExecuteCommandAndExitFromPipeline( const Command& command )
{
	_exit( ExecuteCommandFromPipeline( command ) );
}

static int ExecutePipeline( const Pipeline& pipeline )
{
	std::vector< Command > commands( pipeline.commands.size() );
	
	std::transform( pipeline.commands.begin(),
	                pipeline.commands.end(),
	                commands.begin(),
	                std::ptr_fun( ParseCommand ) );
	
	switch ( commands.size() )
	{
		case 0:
			return 0;
			break;
		case 1:
			{
				return ExecuteCommand( commands.front() );
			}
			break;
	}
	
	typedef std::vector< Command >::const_iterator const_iterator;
	
	const_iterator command = commands.begin();
	
	int pipes[ 2 ];
	
	pipe( pipes );  // pipe between first two processes
	
	int reading = pipes[ 0 ];
	int writing = pipes[ 1 ];
	
	// The first command in the pipline
	int first = vfork();
	
	if ( first == -1 )
	{
		std::perror( "sh: vfork()" );
		
		return 127;
	}
	
	if ( first == 0 )
	{
		// Redirect output to write-end of pipe
		dup2( writing, 1 );
		
		// Close duped write-end and unused read-end
		close( writing );     // we duped this, close it
		close( pipes[ 0 ] );  // we don't read from this pipe, close it
		
		if ( GetOption( kOptionMonitor ) )
		{
			setpgrp();
			
			tcsetpgrp( 0, getpgrp() );
		}
		
		// exec or exit
		ExecuteCommandAndExitFromPipeline( commands.front() );
	}
	
	// previous pipe fd's are saved in 'reading' and 'writing'.
	
	while ( ++command != commands.end() - 1 )
	{
		pipe( pipes );  // next pipe
		
		// Close previous write-end
		close( writing );
		
		writing = pipes[ 1 ];  // write-end of next pipe
		
		// Middle command in the pipeline (not first or last)
		int middle = vfork();
		
		if ( middle == -1 )
		{
			std::perror( "sh: vfork()" );
			
			return 127;
		}
		
		if ( middle == 0 )
		{
			// Redirect input from read-end of previous pipe
			dup2( reading, 0 );
			
			// Redirect output to write-end of next pipe
			dup2( writing, 1 );
			
			// Close duped pipe-in, duped pipe-out, and unused read-end
			close( reading );     // we duped this, close it
			close( writing );     // we duped this, close it
			close( pipes[ 0 ] );  // we don't read from this pipe, close it
			
			setpgid( 0, first );
			
			ExecuteCommandAndExitFromPipeline( *command );
		}
		
		// Child is forked, so we're done reading
		close( reading );
		
		reading = pipes[ 0 ];  // read-end of next pipe
	}
	
	// previous pipe fd's are saved in 'reading' and 'writing'.
	
	// Close previous write-end
	close( writing );
	
	int last = vfork();
	
	if ( last == -1 )
	{
		std::perror( "sh: vfork()" );
		
		return 127;
	}
	
	if ( last == 0 )
	{
		// Redirect input from read-end of previous pipe
		dup2( reading, 0 );
		
		close( reading );     // we duped this, close it
		
		setpgid( 0, first );
		
		ExecuteCommandAndExitFromPipeline( *command );
	}
	
	// Child is forked, so we're done reading
	close( reading );
	
	int processes = commands.size();
	
	int result = -1;
	
	while ( processes )
	{
		int stat = -1;
		
		// Wait for every child process
		pid_t pid = waitpid( -1, &stat, 0 );
		
		--processes;
		
		if ( pid == last )
		{
			// And keep the result from the last one
			result = exit_from_wait( stat );
		}
	}
	
	return result;
}

static bool ShortCircuit( Sh::ControlOperator op, int result )
{
	return ( op == Sh::kControlAnd  &&  result != 0 )
		|| ( op == Sh::kControlOr   &&  result == 0 );
}

static int ExecuteCircuit( const Circuit& circuit )
{
	if ( circuit.op == Sh::kControlBackground )
	{
		Io::Err << "Background jobs are not supported.  Sorry.\n";
		
		return 127;
	}
	
	int result = 0;
	
	typedef std::vector< Pipeline >::const_iterator vP_ci;
	
	for ( vP_ci it = circuit.pipelines.begin();  it != circuit.pipelines.end();  ++it )
	{
		if ( ShortCircuit( it->op, result ) )
		{
			continue;
		}
		
		gLastResult = result = ExecutePipeline( *it );
		
		if ( GetOption( kOptionMonitor ) )
		{
			tcsetpgrp( 0, getpgrp() );
		}
	}
	
	return result;
}

static int ExecuteList( const List& list )
{
	int result = 0;
	
	typedef std::vector< Circuit >::const_iterator vP_ci;
	
	for ( vP_ci it = list.begin();  it != list.end();  ++it )
	{
		result = ExecuteCircuit( *it );
		
		if ( GetOption( kOptionExitOnError )  &&  result != 0 )
		{
			break;
		}
	}
	
	return result;
}

int ExecuteCmdLine( const std::string& cmd )
{
	List list = Sh::Tokenization( cmd );
	
	int result = ExecuteList( list );
	
	return result;
}

