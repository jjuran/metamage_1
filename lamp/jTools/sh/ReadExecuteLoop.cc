// ==================
// ReadExecuteLoop.cc
// ==================

#include "ReadExecuteLoop.hh"

// Standard C/C++
#include <cstring>

// Standard C
#include <stdlib.h>

// POSIX
#include <unistd.h>

// Io
#include "Io/TextInput.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// sh
#include "Execution.hh"
#include "Options.hh"


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

int ReadExecuteLoop( P7::FileDescriptor  fd,
                     bool                prompts )
{
	Io::TextInputAdapter< P7::FileDescriptor > input( fd );
	
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
			
			if ( command == "exit" )
			{
				return 0;
			}
			
			result = ExecuteCmdLine( command );
			
			if ( !GetOption( kOptionInteractive )  &&  GetOption( kOptionExitOnError )  &&  result != 0 )
			{
				break;
			}
			
			if ( prompts )
			{
				SendPrompt();
			}
		}
	}
	
	return result;
}

