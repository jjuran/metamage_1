// ============
// Execution.cc
// ============

#include "Execution.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

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

// debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"
#include "plus/string/concat.hh"

// vxo
#include "vxo/ptrvec.hh"

// poseven
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


extern "C" char** environ;

#ifdef __RELIX__
// Relix
#include "relix/alloca.h"
#define ALLOCA( x )  signalling_alloca( x )
#else
#define ALLOCA( x )  alloca( x )
#endif

#ifdef __RELIX__

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

void psignal( unsigned signo, const char* s )
{
	more::perror( strsignal( signo ), 0 );
}

#endif

class argv_box
{
	private:
		plus::var_string      its_string;
		std::vector< char* >  its_vector;
	
	public:
		char* const* get_argv() const  { return &its_vector[ 0 ]; }
		
		argv_box& assign( const plus::string& s );
};

static
void assign_unflattened_argv( std::vector< char* >& result, plus::var_string& flat )
{
	result.clear();
	
	char* begin = &*flat.begin();
	char* end   = &*flat.end();
	
	while ( begin < end )
	{
		char* null = std::find( begin, end, '\0' );
		
		ASSERT( null != end );
		
		result.push_back( begin );
		
		begin = null + 1;
	}
	
	result.push_back( NULL );
}

argv_box& argv_box::assign( const plus::string& s )
{
	its_string = s;
	
	assign_unflattened_argv( its_vector, its_string );
	
	return *this;
}


namespace tool
{
	
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
	
	
	static inline
	int count_argv( char** argv )
	{
		int result = 0;
		
		while ( *argv++ )
		{
			++result;
		}
		
		return result;
	}
	
	template < class Strings >
	static
	void fill_argv( char** argv, Strings& args )
	{
		for ( size_t i = 0;  i < args.size();  ++i )
		{
			*argv++ = (char*) args[ i ].c_str();
		}
		
		*argv = NULL;
	}
	
	static
	StringVector lookup_shell_param( const plus::string& param, bool double_quoted )
	{
		plus::var_string single_result;
		
		if ( param == "$" )
		{
			single_result = gear::inscribe_decimal( getpid() );
		}
		else if ( param == "?" )
		{
			single_result = gear::inscribe_decimal( exit_from_wait( global_last_wait_status ) );
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
				
				for ( int i = 1;  i < gParameterCount;  ++i )
				{
					single_result += " ";
					single_result += gParameters[ i ];
				}
			}
		}
		else if ( param == "@"  ||  param == "*" )
		{
			StringVector result;
			
			// TODO:  Preallocate gParameterCount slots
			
			for ( size_t i = 0;  i < gParameterCount;  ++i )
			{
				result.push_back( gParameters[ i ] );
			}
			
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
		
		if ( opened < 0 )
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
		
		if ( status < 0  ||  S_ISREG( sb.st_mode ) )
		{
			// Either the stat failed and something's wrong, or a regular file exists.
			int error = status < 0 ? errno : EEXIST;
			
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
		
		if ( status < 0  ||  S_ISREG( sb.st_mode ) )
		{
			// Either the stat failed and something's wrong, or a regular file exists.
			int error = status < 0 ? errno : EEXIST;
			
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
					
					if ( fd < 0 )
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
		for ( size_t i = 0;  i < redirections.size();  ++i )
		{
			RedirectIO( redirections[ i ] );
		}
	}
	
	static
	void Exec( char const* const argv[], const argv_box& env )
	{
		char const* const* envp = env.get_argv();
		
		const char* file = argv[ 0 ];
		
		(void) execvpe( file, (char**) argv, (char**) envp );
		
		const char* error_msg = errno == ENOENT ? "command not found" : strerror( errno );
		
		more::perror( "sh", file, error_msg );
		
		_exit( errno == ENOENT ? 127 : 126 );  // Use _exit() to exit a forked but not exec'ed process.
	}
	
	
	static p7::exit_t CallBuiltin( Builtin builtin, char** argv )
	{
		int argc = count_argv( argv );
		
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
		
		while ( *argv != NULL  &&  strchr( *argv, '=' ) )
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
			char* eq = strchr( *argv, '=' );
			
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
		vxo::UPtrVec< const char > env;
		
		env_less less;
		
		if ( environ )
		{
			// Assume environ has no duplicate variable names
			
			for ( char** envp = environ;  *envp != NULL;  ++envp )
			{
				env.push_back( *envp );
			}
			
			std::sort( env.begin(), env.end(), less );
		}
		
		const char* arg;
		
		while ( char* eq = (char*) strchr( arg = *argv++, '=' ) )
		{
			const char** lb = std::lower_bound( env.begin(),
			                                    env.end(),
			                                    arg,
			                                    less );
			
			if ( lb == env.end()  ||  less( arg, *lb ) )
			{
				lb = env.insert( lb );
			}
			
			*lb = arg;
		}
		
		--argv;
		
		plus::var_string result;
		
		typedef vxo::UPtrVec< const char >::const_iterator Iter;
		
		for ( Iter it = env.begin();  it != env.end();  ++it )
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
			signal( SIGINT,  SIG_DFL );
			signal( SIGQUIT, SIG_DFL );
			signal( SIGTSTP, SIG_DFL );
			signal( SIGTTIN, SIG_DFL );
			signal( SIGTTOU, SIG_DFL );
			
			setpgid( 0, pgid );
			
			tcsetpgrp( 0, getpgrp() );
		}
	}
	
	static
	p7::wait_t ExecuteCommand( Command& command )
	{
		const size_t argc = command.args.size();
		
		char** argv = (char**) ALLOCA( (argc + 1) * sizeof (char*) );
		
		fill_argv( argv, command.args );
		
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
			
			argv_box env;
			
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
				throw exit_from_wait( wait_status );
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
	
	
	static
	p7::wait_t ExecuteCommandFromPipeline( Command& command, argv_box& env )
	{
		const size_t argc = command.args.size();
		
		char** argv = (char**) ALLOCA( (argc + 1) * sizeof (char*) );
		
		fill_argv( argv, command.args );
		
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
	
	static
	void ExecuteCommandAndExitFromPipeline( Command& command, argv_box& env )
	{
		try
		{
			p7::_exit( exit_from_wait( ExecuteCommandFromPipeline( command, env ) ) );
		}
		catch ( const p7::exit_t& status )
		{
			p7::_exit( status );
		}
	}
	
	static p7::wait_t ExecutePipeline( const Pipeline& pipeline )
	{
		std::vector< Command > commands;
		
		const size_t n = pipeline.commands.size();
		
		commands.reserve( n );
		
		for ( size_t i = 0;  i != n;  ++i )
		{
			commands.push_back( ParseCommand( pipeline.commands[ i ] ) );
		}
		
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
		
		argv_box env;
		
		typedef std::vector< Command >::iterator iterator;
		
		iterator command = commands.begin();
		
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
			
			psignal( signo, "" );  // Mac OS X 10.2 doesn't NULL-check
		}
		
		return status;
	}
	
	p7::wait_t ExecuteCmdLine( const char* cmd )
	{
		return ExecuteCmdLine( plus::string( cmd ) );
	}
	
}
