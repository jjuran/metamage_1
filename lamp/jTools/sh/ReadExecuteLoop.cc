// ==================
// ReadExecuteLoop.cc
// ==================

#include "ReadExecuteLoop.hh"

// Standard C/C++
#include <cstring>

// Standard C
#include <stdlib.h>

// POSIX
#include <sys/ioctl.h>
#include <unistd.h>

// Lamp
#ifdef __LAMP__
#include "lamp/winio.h"
#endif

// POSeven
#include "POSeven/Open.hh"

// Io
#include "Io/TextInput.hh"

// Orion
#include "Orion/Main.hh"

// sh
#include "Builtins.hh"
#include "Execution.hh"
#include "Options.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
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
		
		p7::write( p7::stdout_fileno, prompt_string, std::strlen( prompt_string ) );
	}
	
	static void SetRowsAndColumns()
	{
	#ifdef WIOCGDIM
		
		// FIXME:  Use fallback for real Unix
		
		try
		{
			NN::Owned< p7::fd_t > tty = p7::open( "/dev/tty", p7::o_rdonly, 0 );
			
			short dimensions[ 2 ] = { 0 };
			
			int status = ioctl( tty, WIOCGDIM, &dimensions );
			
			if ( status == 0 )
			{
				std::string lines   = NN::Convert< std::string >( dimensions[0] );
				std::string columns = NN::Convert< std::string >( dimensions[1] );
				
				AssignShellVariable( "LINES",   lines  .c_str() );
				AssignShellVariable( "COLUMNS", columns.c_str() );
			}
		}
		catch ( ... )
		{
		}
		
	#endif
	}
	
	p7::wait_t ReadExecuteLoop( p7::fd_t  fd,
	                            bool      prompts )
	{
		Io::TextInputAdapter< p7::fd_t > input( fd );
		
		int result = 0;
		
		if ( prompts )
		{
			std::printf( "Shell spawned with pid %d\n", getpid() );
			
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
						return p7::wait_t( 0 );
					}
					
					SetRowsAndColumns();
					
					result = ExecuteCmdLine( command );
					
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
		
		return p7::wait_t( result );
	}
	
}

