// ============
// Execution.cc
// ============

#include "Execution.hh"

// Standard C++
#include <algorithm>
#include <map>
#include <set>

// Standard C/C++
#include <cstring>

// Standard C
#include <errno.h>
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// must
#include "must/pipe.h"

// iota
#include "iota/char_types.hh"
#include "iota/strings.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// more-posix
#include "more/perror.hh"

// plus
#include "plus/argv.hh"
#include "plus/var_string.hh"
#include "plus/string/concat.hh"

// poseven
#include "poseven/functions/sigaction.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/wait.hh"
#include "poseven/functions/write.hh"
#include "poseven/functions/_exit.hh"

// sh
#include "Builtins.hh"
#include "execvpe.hh"
#include "Expansion.hh"
#include "Options.hh"
#include "PositionalParameters.hh"
#include "StringArray.hh"


extern "C" char** environ;

#ifdef __MWERKS__

static const char* global_signal_names[] =
{
	"Signal 0",
	"Hangup",
	"",  // INT
	"Quit",
	"Illegal instruction",
	"Trace/breakpoint trap",
	"Aborted",
	"Bus error",
	"Floating point exception",
	"Killed",
	"User defined signal 1",
	"Segmentation fault",
	"User defined signal 2",
	"Broken pipe",
	"Alarm clock",
	"Terminated",
	"Stack fault",
	"Child exited",
	"Continued",
	"Stopped (signal)",
	"Stopped",
	"Stopped (tty input)",
	"Stopped (tty output)",
	"Urgent I/O condition",
	"CPU time limit exceeded",
	"File size limit exceeded",
	"Virtual timer expired",
	"Profiling timer expired",
	"Window changed",
	"I/O possible",
	"Power failure",
	"Bad system call",
	NULL
};

const char* strsignal( int signo )
{
	if ( signo < 0  ||  signo > NSIG )
	{
		return "BAD SIGNAL NUMBER";
	}
	
	return global_signal_names[ signo ];
}

#endif


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace Sh = ShellShock;
	
	
	using Sh::Command;
	using Sh::Pipeline;
	using Sh::Circuit;
	using Sh::List;
	
	
	static p7::wait_t global_last_wait_status;
	
	
	static inline p7::wait_t wait_from_exit( p7::exit_t exit_status )
	{
		return p7::wait_t( exit_status << 8 );
	}
	
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
			AppendWithSpace( plus::var_string& ioString ) : myString( ioString )  {}
			
			void operator()( const plus::string& inNew )
			{
				myString += " ";
				myString += inNew;
			}
		
		private:
			plus::var_string& myString;
	};
	
	
	template < class T >
	static inline std::vector< T > MakeVector( const T& value )
	{
		return std::vector< T >( 1, value );
	}
	
	static std::vector< plus::string > lookup_shell_param( const plus::string& param, bool double_quoted )
	{
		plus::var_string single_result;
		
		if ( param == "$" )
		{
			single_result = gear::inscribe_decimal( getpid() );
		}
		else if ( param == "?" )
		{
			single_result = gear::inscribe_decimal( n::convert< p7::exit_t >( global_last_wait_status ) );
		}
		else if ( param == "#" )
		{
			single_result = gear::inscribe_decimal( gParameterCount );
		}
		else if ( double_quoted  &&  param == "*" )
		{
			if ( gParameterCount != 0 )
			{
				single_result = gParameters[ 0 ];
				
				std::for_each( gParameters + 1,
				               gParameters + gParameterCount,
				               AppendWithSpace( single_result ) );
			}
		}
		else if ( param == "@"  ||  param == "*" )
		{
			std::vector< plus::string > result( gParameterCount );
			
			std::copy( gParameters, gParameters + gParameterCount, result.begin() );
			
			return result;
		}
		else if ( iota::is_digit( param[0] ) )
		{
			int i = gear::parse_unsigned_decimal( param.c_str() );
			
			if ( i <= gParameterCount )
			{
				single_result = gParameters[ i - 1 ];
			}
		}
		else if ( param == "0" )
		{
			single_result = gArgZero;
		}
		else if ( const char* value = QueryShellVariable( param ) )
		{
			single_result = value;
		}
		else if ( const char* var = getenv( param.c_str() ) )
		{
			single_result = var;
		}
		
		const plus::string& const_result = single_result;
		
		return MakeVector( const_result );
	}
	
	static int Open( const char* path, int oflags )
	{
		int opened = open( path, oflags, 0666 );
		
		p7::throw_posix_result( opened );
		
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
				p7::throw_errno( errno );
			}
			
			// No such file, exclusively create it, or bust.
			opened = Open( path, O_WRONLY | O_CREAT | O_EXCL );
			
			// Return exclusively created file.
			return opened;
		}
		
		struct stat sb;
		
		int status = fstat( opened, &sb );
		
		if ( status == -1  ||  S_ISREG( sb.st_mode ) )
		{
			// Either the stat failed and something's wrong, or a regular file exists.
			int error = status == -1 ? errno : EEXIST;
			
			// Close the file.
			int closed = close( opened );
			
			p7::throw_errno( error );
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
			p7::throw_errno( errno );  // Some error occurred (besides an existing file)
		}
		
		// File exists.  Try again without creating.
		opened = Open( path, O_WRONLY );
		
		struct stat sb;
		
		int status = fstat( opened, &sb );
		
		if ( status == -1  ||  S_ISREG( sb.st_mode ) )
		{
			// Either the stat failed and something's wrong, or a regular file exists.
			int error = status == -1 ? errno : EEXIST;
			
			// Close the file.
			int closed = close( opened );
			
			p7::throw_errno( error );
		}
		
		// Return existing non-regular file -- maybe a device/pipe/socket, etc.
		return opened;
	}
	*/
	
	static void Dup2( int oldfd, int newfd )
	{
		p7::throw_posix_result( dup2( oldfd, newfd ) );
	}
	
	static void dup2_and_close( int oldfd, int newfd )
	{
		if ( oldfd != newfd )
		{
			Dup2( oldfd, newfd );
			
			close( oldfd );
		}
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
				case Sh::kRedirectNone:
					break;
				
				case Sh::kRedirectInput:
					file = Open( param, O_RDONLY );
					
					dup2_and_close( file, fd );
					break;
				
				case Sh::kRedirectInputHere:
				case Sh::kRedirectInputHereStrippingTabs:
					break;
				
				case Sh::kRedirectInputDuplicate:
				case Sh::kRedirectOutputDuplicate:
					if ( param[0] == '-'  &&  param[1] == '\0' )
					{
						close( fd );
						break;
					}
					
					file = gear::parse_unsigned_decimal( param );  // FIXME:  Probably bad if param isn't an integer
					Dup2( file, fd );
					break;
				
				case Sh::kRedirectInputAndOutput:
					file = Open( param, O_RDWR );
					
					if ( fd == -1 )
					{
						Dup2( file, 0 );
						Dup2( file, 1 );
						
						close( file );
					}
					else
					{
						dup2_and_close( file, fd );
					}
					
					break;
				
				case Sh::kRedirectOutput:
					if ( GetOption( kOptionNonClobberingRedirection ) )
					{
						file = OpenNoClobber( param );
						
						dup2_and_close( file, fd );
						
						break;
					}
					// else fall through
					
				case Sh::kRedirectOutputClobbering:
					file = Open( param, O_WRONLY | O_CREAT | O_TRUNC );
					
					dup2_and_close( file, fd );
					break;
				
				case Sh::kRedirectOutputAppending:
					file = Open( param, O_WRONLY | O_APPEND | O_CREAT );
					
					dup2_and_close( file, fd );
					break;
				
				case Sh::kRedirectOutputAndError:
					file = Open( param, O_WRONLY | O_CREAT | O_TRUNC );
					Dup2( file, 1 );
					Dup2( file, 2 );
					close( file );
					break;
			}  // switch
		}
		catch ( const p7::errno_t& errnum )
		{
			more::perror( "sh", param, errnum );
			
			throw;
		}
	}
	
	static void RedirectIOs( const std::vector< Sh::Redirection >& redirections )
	{
		std::for_each( redirections.begin(),
					   redirections.end(),
					   std::ptr_fun( RedirectIO ) );
	}
	
	static void Exec( char const* const argv[], char const* const* envp )
	{
		const char* file = argv[ 0 ];
		
		(void) execvpe( file, (char**) argv, (char**) envp );
		
		const char* error_msg = errno == ENOENT ? "command not found" : std::strerror( errno );
		
		more::perror( "sh", file, error_msg );
		
		_exit( errno == ENOENT ? 127 : 126 );  // Use _exit() to exit a forked but not exec'ed process.
	}
	
	static void Exec( char const* const argv[], plus::argv& env )
	{
		Exec( argv, env.get_argv() );
	}
	
	
	static p7::exit_t CallBuiltin( Builtin builtin, char** argv )
	{
		int argc = Sh::CountStringArray( argv );
		
		return builtin( argc, argv );
	}
	
	static plus::string EscapeForShell( const char* word )
	{
		plus::var_string result;
		
		for ( const char* p = word;  *p != '\0';  ++p )
		{
			if ( !iota::is_alnum( *p ) )
			{
				result += '\\';
			}
			
			result += *p;
		}
		
		return result;
	}
	
	static plus::string MakeShellCommandFromBuiltin( char** argv )
	{
		plus::var_string command = *argv;
		
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
	
	static p7::wait_t AssignShellVariablesFromArgV( char** argv )
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
		
		return p7::wait_t( 0 );
	}
	
	struct env_less
	{
		bool operator()( const char* a, const char* b ) const
		{
			while ( true )
			{
				const bool a_ended = *a == '\0'  ||  *a == '=';
				const bool b_ended = *b == '\0'  ||  *b == '=';
				
				if ( a_ended  ||  b_ended )
				{
					return a_ended > b_ended;
				}
				
				if ( *a != *b )
				{
					typedef unsigned char uint8_t;
					
					return uint8_t( *a ) < uint8_t( *b );
				}
				
				++a;
				++b;
			}
		}
	};
	
	static plus::string ShiftEnvironmentVariables( char**& argv )
	{
		//ASSERT( argv != NULL );
		
		std::set< const char*, env_less > set;
		
		if ( environ )
		{
			for ( char** envp = environ;  *envp != NULL;  ++envp )
			{
				set.erase ( *envp );
				set.insert( *envp );
			}
		}
		
		while ( char* eq = std::strchr( argv[ 0 ], '=' ) )
		{
			set.erase ( argv[ 0 ] );
			set.insert( argv[ 0 ] );
			
			++argv;
		}
		
		plus::var_string result;
		
		typedef std::set< const char*, env_less >::const_iterator Iter;
		
		for ( Iter it = set.begin();  it != set.end();  ++it )
		{
			const char* var = *it;
			
			result.append( var, strlen( var ) + 1 );
		}
		
		return result.move();
	}
	
	static Command ParseCommand( const Command& command )
	{
		return Sh::ParseCommand( command, &lookup_shell_param );
	}
	
	static void SetupChildProcess( p7::pid_t pgid = p7::pid_t( 0 ) )
	{
		if ( GetOption( kOptionMonitor ) )
		{
			p7::sigaction( p7::sigint,  p7::sig_dfl );
			p7::sigaction( p7::sigquit, p7::sig_dfl );
			p7::sigaction( p7::sigtstp, p7::sig_dfl );
			p7::sigaction( p7::sigttin, p7::sig_dfl );
			p7::sigaction( p7::sigttou, p7::sig_dfl );
			
			setpgid( 0, pgid );
			
			tcsetpgrp( 0, getpgrp() );
		}
	}
	
	static p7::wait_t ExecuteCommand( const Command& command )
	{
		Sh::StringArray argvec( command.args );
		
		char** argv = argvec.GetPointer();
		
		if ( CommandIsOnlyAssignments( argv ) )
		{
			return AssignShellVariablesFromArgV( argv );
		}
		
		Builtin builtin = FindBuiltin( argv[ 0 ] );
		
		try
		{
			if ( builtin != NULL  &&  ((strcmp( argv[0], "exec" ) == 0  &&  (RedirectIOs( command.redirections ), true))  ||  command.redirections.empty()) )
			{
				return wait_from_exit( CallBuiltin( builtin, argv ) );  // wait from exit
			}
			
			plus::argv env;
			
			// This variable is set before and examined after a longjmp(), so it
			// needs to be volatile to make sure it doesn't wind up in a register
			// and subsequently clobbered.
			volatile bool exiting = false;
			
			p7::pid_t pid = POSEVEN_VFORK();
			
			if ( pid == 0 )
			{
				SetupChildProcess();
				
				try
				{
					RedirectIOs( command.redirections );
					
					if ( builtin != NULL )
					{
						try
						{
							// Since we didn't actually exec anything, we have to exit manually
							p7::_exit( CallBuiltin( builtin, argv ) );
						}
						catch ( const p7::exit_t& status )
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
						env.assign( ShiftEnvironmentVariables( argv ) );
						
						Exec( argv, env );
					}
					
					// Not reached
				}
				catch ( ... )
				{
					// Don't unwind past vfork().  That would be Bad.
				}
				
				// If the child hasn't exited by now, do so.
				p7::_exit( p7::exit_failure );
			}
			
			// Wait for the child process to exit
			p7::wait_t wait_status = p7::wait();
			
			if ( exiting )
			{
				// The 'child' was the 'exit' builtin, meaning we should exit
				throw n::convert< p7::exit_t >( wait_status );
			}
			
			return wait_status;
		}
		catch ( const p7::exit_t& )
		{
			// Rethrow.  We catch here so a thrown exit doesn't get caught below.
			throw;
		}
		catch ( ... )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "sh: exception occurred running command\n" ) );
		}
		
		return wait_from_exit( p7::exit_failure );
	}
	
	
	static p7::wait_t ExecuteCommandFromPipeline( const Command& command, plus::argv& env )
	{
		Sh::StringArray argvec( command.args );
		
		char** argv = argvec.GetPointer();
		
		if ( CommandIsOnlyAssignments( argv ) )
		{
			// An assignment as the sole command to a shell is pointless, so just exit
			return p7::wait_t( 0 );
		}
		
		try
		{
			RedirectIOs( command.redirections );
			
			env.assign( ShiftEnvironmentVariables( argv ) );
			
			if ( Builtin builtin = FindBuiltin( argv[ 0 ] ) )
			{
				plus::string subshell = MakeShellCommandFromBuiltin( argv );
				
				const char* subshell_argv[] = { "/bin/sh", "-c", subshell.c_str(), NULL };
				
				Exec( subshell_argv, env );
			}
			
			Exec( argv, env );
			
		}
		catch ( const p7::exit_t& status )
		{
			throw;
		}
		catch ( ... )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "sh: exception occurred running command\n" ) );
		}
		
		return wait_from_exit( p7::exit_failure );
	}
	
	static void ExecuteCommandAndExitFromPipeline( const Command& command, plus::argv& env )
	{
		try
		{
			p7::_exit( n::convert< p7::exit_t >( ExecuteCommandFromPipeline( command, env ) ) );
		}
		catch ( const p7::exit_t& status )
		{
			p7::_exit( status );
		}
	}
	
	static p7::wait_t ExecutePipeline( const Pipeline& pipeline )
	{
		std::vector< Command > commands( pipeline.commands.size() );
		
		std::transform( pipeline.commands.begin(),
		                pipeline.commands.end(),
		                commands.begin(),
		                std::ptr_fun( ParseCommand ) );
		
		switch ( commands.size() )
		{
			case 0:
				return p7::wait_t( 0 );
				break;
			
			case 1:
				{
					return ExecuteCommand( commands.front() );
				}
				break;
		}
		
		plus::argv env;
		
		typedef std::vector< Command >::const_iterator const_iterator;
		
		const_iterator command = commands.begin();
		
		int pipes[ 2 ];
		
		must_pipe( pipes );  // pipe between first two processes
		
		int reading = pipes[ 0 ];
		int writing = pipes[ 1 ];
		
		// The first command in the pipline
		p7::pid_t first = POSEVEN_VFORK();
		
		if ( first == 0 )
		{
			// Redirect output to write-end of pipe
			dup2( writing, 1 );
			
			// Close duped write-end and unused read-end
			close( writing );     // we duped this, close it
			close( pipes[ 0 ] );  // we don't read from this pipe, close it
			
			SetupChildProcess();
			
			// exec or exit
			ExecuteCommandAndExitFromPipeline( commands.front(), env );
		}
		
		// previous pipe fd's are saved in 'reading' and 'writing'.
		
		while ( ++command != commands.end() - 1 )
		{
			must_pipe( pipes );  // next pipe
			
			// Close previous write-end
			close( writing );
			
			writing = pipes[ 1 ];  // write-end of next pipe
			
			// Middle command in the pipeline (not first or last)
			p7::pid_t middle = POSEVEN_VFORK();
			
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
				
				SetupChildProcess( first );
				
				ExecuteCommandAndExitFromPipeline( *command, env );
			}
			
			// Child is forked, so we're done reading
			close( reading );
			
			reading = pipes[ 0 ];  // read-end of next pipe
		}
		
		// previous pipe fd's are saved in 'reading' and 'writing'.
		
		// Close previous write-end
		close( writing );
		
		p7::pid_t last = POSEVEN_VFORK();
		
		if ( last == 0 )
		{
			// Redirect input from read-end of previous pipe
			dup2( reading, 0 );
			
			close( reading );     // we duped this, close it
			
			SetupChildProcess( first );
			
			ExecuteCommandAndExitFromPipeline( *command, env );
		}
		
		// Child is forked, so we're done reading
		close( reading );
		
		int processes = commands.size();
		
		p7::wait_t wait_status = p7::wait_t( -1 );
		
		while ( processes )
		{
			p7::wait_t stat = p7::wait_t( -1 );
			
			// Wait for every child process
			p7::pid_t pid = p7::wait( stat );
			
			--processes;
			
			if ( pid == last )
			{
				// And keep the result from the last one
				wait_status = stat;
			}
		}
		
		return wait_status;
	}
	
	static bool ShortCircuit( Sh::ControlOperator op, int result )
	{
		return ( op == Sh::kControlAnd  &&  result != 0 )
			|| ( op == Sh::kControlOr   &&  result == 0 );
	}
	
	static p7::wait_t ExecuteCircuit( const Circuit& circuit )
	{
		if ( circuit.op == Sh::kControlBackground )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Background jobs are not supported.  Sorry.\n" ) );
			
			return wait_from_exit( p7::exit_failure );
		}
		
		p7::wait_t status = p7::wait_t( 0 );
		
		typedef std::vector< Pipeline >::const_iterator vP_ci;
		
		for ( vP_ci it = circuit.pipelines.begin();  it != circuit.pipelines.end();  ++it )
		{
			if ( ShortCircuit( it->op, status ) )
			{
				continue;
			}
			
			try
			{
				status = ExecutePipeline( *it );
			}
			catch ( const p7::errno_t& )
			{
				status = wait_from_exit( p7::exit_failure );
			}
			
			global_last_wait_status = status;
			
			if ( GetOption( kOptionMonitor ) )
			{
				tcsetpgrp( 0, getpgrp() );
			}
		}
		
		return status;
	}
	
	static p7::wait_t ExecuteList( const List& list )
	{
		p7::wait_t status = p7::wait_t( 0 );
		
		typedef std::vector< Circuit >::const_iterator vP_ci;
		
		for ( vP_ci it = list.begin();  it != list.end();  ++it )
		{
			status = ExecuteCircuit( *it );
			
			if ( GetOption( kOptionExitOnError )  &&  status != 0 )
			{
				break;
			}
		}
		
		return status;
	}
	
	p7::wait_t ExecuteCmdLine( const plus::string& cmd )
	{
		List list = Sh::Tokenization( cmd );
		
		p7::wait_t status = ExecuteList( list );
		
		// notify user of fatal signal, e.g. "Alarm clock"
		if ( p7::wifsignaled( status ) )
		{
			int signo = p7::wtermsig( status );
			
			more::perror( strsignal( signo ), 0 );
		}
		
		return status;
	}
	
	p7::wait_t ExecuteCmdLine( const char* cmd )
	{
		return ExecuteCmdLine( plus::string( cmd ) );
	}
	
}
