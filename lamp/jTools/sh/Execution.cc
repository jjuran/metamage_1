// ============
// Execution.cc
// ============

#include "Execution.hh"

// Standard C/C++
#include <cstring>

// Standard C
#include "errno.h"
#include "stdlib.h"

// POSIX
#include "fcntl.h"
#include "sys/wait.h"
#include "unistd.h"

// BSD
#include "vfork.h"

// Io
#include "Io/TextInput.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// ShellShock
#include "ShellShock/Expansion.hh"
#include "ShellShock/StringArray.hh"

// sh
#include "Builtins.hh"
#include "PositionalParameters.hh"


namespace NN = Nucleus;
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

static int NewJob( const Circuit& circuit )
{
	int job = NextJobNumber();
	
	gJobTable[ job ];
	
	return job;
	
	
	
}


static StringMap gLocalVariables;


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
	
	StringMap::const_iterator found = gLocalVariables.find( param );
	
	if ( found != gLocalVariables.end() )
	{
		return found->second;
	}
	else if ( const char* var = getenv( param.c_str() ) )
	{
		return var;
	}
	
	return "";
}


static void RedirectIO( Sh::Redirection redirection )
{
	int fd = redirection.fd;
	const char* param = redirection.param.c_str();
	
	int file = -1;
	
	switch ( redirection.op )
	{
		case Sh::kRedirectInput:
			dup2( open( param, 0 ), fd );
			break;
		
		case Sh::kRedirectInputHere:
		case Sh::kRedirectInputHereStrippingTabs:
			break;
		
		case Sh::kRedirectInputDuplicate:
			dup2( std::atoi( param ), fd );  // FIXME:  Probably bad if param isn't an integer
			break;
		
		case Sh::kRedirectInputAndOutput:
			file = open( param, 2 );
			
			if ( fd == -1 )
			{
				dup2( file, 0 );
				dup2( file, 1 );
			}
			else
			{
				dup2( file, fd );
			}
			break;
		
		case Sh::kRedirectOutput:
			// Throw if noclobber
		case Sh::kRedirectOutputClobbering:
			dup2( open( param, 1 | 0x0200 | 0x0400 ), fd );
			break;
		
		case Sh::kRedirectOutputAppending:
			dup2( open( param, 1 | 8 | 0x0200 ), fd );
			break;
		
		case Sh::kRedirectOutputDuplicate:
			dup2( std::atoi( param ), fd );  // FIXME:  Probably bad if atoi returns 0
			break;
		
		case Sh::kRedirectOutputAndError:
			file = open( param, 1 | 0x0200 | 0x0400 );
			dup2( file, 1 );
			dup2( file, 2 );
			break;
	}  // switch
}

static int Exec( char const* const argv[] )
{
	const char* file = argv[ 0 ];
	
	int exec_result = execvp( file, const_cast< char** >( argv ) );
	
	if ( exec_result != -1 )
	{
		return exec_result;
	}
	
	int error = errno;
	
	Io::Err << "execvp( " << file << " ) failed: " << error << "\n";
	
	_exit( 1 );  // Use _exit() to exit a forked but not exec'ed process.
	
	return -1;
}

static int Wait( pid_t pid )
{
	int stat = -1;
	
	pid_t resultpid = waitpid( pid, &stat, 0 );
	
	if ( resultpid == -1 )
	{
		Io::Err << "waitpid() failed\n";
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


static int ExecuteCommand( const Command& command )
{
	Sh::StringArray argvec( command.args );
	
	char** argv = argvec.GetPointer();
	
	if ( argv[ 0 ] == NULL )
	{
		return 0;
	}
	
	if ( char* eq = std::strchr( argv[ 0 ], '=' ) )
	{
		*eq = '\0';
		
		const char* name = argv[ 0 ];
		
		return Assign( name, eq + 1 );
	}
	
	try
	{
		if ( Builtin builtin = FindBuiltin( argv[ 0 ] ) )
		{
			SetInputFD();
			SetOutputFD();
			SetErrorFD();
			
			int argc = Sh::CountStringArray( argv );
			
			return builtin( argc, argv );
		}
		
		int pid = vfork();
		
		if ( pid == 0 )
		{
			std::for_each( command.redirections.begin(),
			               command.redirections.end(),
			               std::ptr_fun( RedirectIO ) );
			
			Exec( argv );
		}
		
		return exit_from_wait( Wait( pid ) );
	}
	catch ( O::ExitStatus )
	{
		throw;
	}
	catch ( ... )
	{
		Io::Err << "wish: An exception occurred while running the command.\n";
	}
	
	return 1;
}


static int ExecuteCommandFromPipeline( const Command& command )
{
	Sh::StringArray argvec( command.args );
	
	char** argv = argvec.GetPointer();
	
	if ( argv[ 0 ] == NULL )
	{
		return 0;
	}
	
	if ( std::strchr( argv[ 0 ], '=' ) )
	{
		// An assignment as the sole command to a shell is pointless, so just exit
		_exit( 0 );
		
		return 0;
	}
	
	try
	{
		if ( Builtin builtin = FindBuiltin( argv[ 0 ] ) )
		{
			SetInputFD();
			SetOutputFD();
			SetErrorFD();
			
			int argc = Sh::CountStringArray( argv );
			
			// Don't exec a new shell, just call the builtin inside the forked child
			_exit( builtin( argc, argv ) );
			
			return 0;
		}
		
		std::for_each( command.redirections.begin(),
		               command.redirections.end(),
		               std::ptr_fun( RedirectIO ) );
		
		return Exec( argv );
		
	}
	catch ( O::ExitStatus )
	{
		throw;
	}
	catch ( ... )
	{
		Io::Err << "wish: An exception occurred while running the command.\n";
	}
	
	return 1;
}


static int ExecutePipeline( const Pipeline& pipeline )
{
	const std::vector< Command >& commands = pipeline.commands;
	
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
	
	pipe( pipes );
	
	int reading = pipes[ 0 ];
	int writing = pipes[ 1 ];
	
	// The first command in the pipline
	int first = vfork();
	
	if ( first == 0 )
	{
		// Redirect output to write-end of pipe
		dup2( writing, 1 );
		
		// Close duped write-end and unused read-end
		close( writing );
		close( pipes[ 0 ] );
		
		setpgrp();
		
		// exec or exit
		ExecuteCommandFromPipeline( commands.front() );
	}
	
	while ( ++command != commands.end() - 1 )
	{
		pipe( pipes );
		
		// Close previous write-end
		close( writing );
		
		writing = pipes[ 1 ];
		
		// Middle command in the pipeline (not first or last)
		int middle = vfork();
		
		if ( middle == 0 )
		{
			// Redirect input from read-end of pipe
			dup2( reading, 0 );
			
			// Redirect output to write-end of pipe
			dup2( writing, 1 );
			
			// Close duped pipe-in, duped pipe-out, and unused read-end
			close( reading );
			close( writing );
			close( pipes[ 0 ] );
			
			setpgid( 0, first );
			
			ExecuteCommandFromPipeline( *command );
		}
		
		close( reading );
		reading = pipes[ 0 ];
	}
	
	int last = vfork();
	
	if ( last == 0 )
	{
		dup2( reading, 0 );
		
		close( reading );
		close( pipes[ 1 ] );
		
		setpgid( 0, first );
		
		ExecuteCommandFromPipeline( *command );
	}
	
	close( pipes[ 0 ] );
	close( pipes[ 1 ] );
	
	int processes = commands.size();
	
	int result = -1;
	
	while ( processes )
	{
		int stat = -1;
		pid_t pid = waitpid( -1, &stat, 0 );
		
		--processes;
		
		if ( pid == last )
		{
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
		O::ThrowExitStatus( 1 );
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
	}
	
	return result;
}

int ExecuteCmdLine( const std::string& cmd )
{
	List list = Sh::ParseCommandLine( cmd, ShellParameterDictionary() );
	
	int result = ExecuteList( list );
	
	return result;
}

