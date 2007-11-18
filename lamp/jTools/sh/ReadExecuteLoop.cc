// ==================
// ReadExecuteLoop.cc
// ==================

#include "ReadExecuteLoop.hh"

// Standard C/C++
#include <cstring>

// Standard C
#include <stdlib.h>

// POSIX
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>

// Io
#include "Io/TextInput.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// sh
#include "Execution.hh"
#include "Options.hh"


namespace p7 = poseven;


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

static const char* get_signal_name( int signo )
{
	if ( signo < 0  ||  signo > NSIG )
	{
		return "BAD SIGNAL NUMBER";
	}
	
	return global_signal_names[ signo ];
}

static PromptLevel gPromptLevel = kPS1;

void SetPromptLevel( PromptLevel level )
{
	gPromptLevel = level;
}

struct Prompt
{
	const char*  environmentVariableName;
	const char*  defaultValue;
};

static const Prompt gPrompts[] =
{
	{ "", "" },
	{ "PS1", "$ " },
	{ "PS2", "> " },
	{ "PS3", "" },
	{ "PS4", "" },
};

static void SendPrompt()
{
	const Prompt& prompt = gPrompts[ gPromptLevel ];
	
	const char* prompt_string = getenv( prompt.environmentVariableName );
	
	if ( prompt_string == NULL )
	{
		prompt_string = prompt.defaultValue;
	}
	
	Io::Out << prompt_string;
}

int ReadExecuteLoop( p7::fd_t  fd,
                     bool      prompts )
{
	Io::TextInputAdapter< p7::fd_t > input( fd );
	
	int result = 0;
	
	if ( prompts )
	{
		Io::Out << "Shell spawned with pid " << getpid() << "\n";
		
		SendPrompt();
	}
	
	while ( !input.Ended() )
	{
		if ( input.Ready() )
		{
			std::string command = input.Read();
			
			// Only process non-blank lines
			if ( command.find_first_not_of( " \t" ) != command.npos )
			{
				if ( command == "exit" )
				{
					return 0;
				}
				
				result = ExecuteCmdLine( command );
				
				// notify user of fatal signal, e.g. "Alarm clock"
				if ( WIFSIGNALED( result ) )
				{
					int signo = WTERMSIG( result );
					
					std::fprintf( stderr, "%s\n", get_signal_name( signo ) );
				}
				
				if ( !GetOption( kOptionInteractive )  &&  GetOption( kOptionExitOnError )  &&  result != 0 )
				{
					break;
				}
			}
			
			if ( prompts )
			{
				SendPrompt();
			}
		}
	}
	
	return result;
}

