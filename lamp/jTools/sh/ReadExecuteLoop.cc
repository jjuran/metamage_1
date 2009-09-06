// ==================
// ReadExecuteLoop.cc
// ==================

#include "ReadExecuteLoop.hh"

// Standard C/C++
#include <cstring>

// Standard C
#include <stdlib.h>

// Iota
#include "iota/decimal.hh"

// POSeven
#include "POSeven/FileDescriptor.hh"
#include "POSeven/functions/ioctl.hh"
#include "POSeven/functions/open.hh"
#include "POSeven/functions/write.hh"

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
		try
		{
			struct winsize size = { 0 };
			
			p7::ioctl( p7::open( "/dev/tty", p7::o_rdonly ).get(),
			           TIOCGWINSZ,
			           &size );
			
			std::string lines   = iota::inscribe_decimal( size.ws_row );
			std::string columns = iota::inscribe_decimal( size.ws_col );
			
			AssignShellVariable( "LINES",   lines  .c_str() );
			AssignShellVariable( "COLUMNS", columns.c_str() );
		}
		catch ( ... )
		{
		}
	}
	
	p7::wait_t ReadExecuteLoop( p7::fd_t  fd,
	                            bool      prompts )
	{
		Io::TextInputAdapter< p7::fd_t > input( fd );
		
		p7::wait_t status = p7::wait_t( 0 );
		
		if ( prompts )
		{
			const char* in = " in ";
			
			const char* window = getenv( "WINDOW" );
			
			if ( window == NULL )
			{
				in     =
				window = "";
			}
			
			std::printf( "Shell spawned with pid %d%s%s\n", getpid(), in, window );
			
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
					
					status = ExecuteCmdLine( command );
					
					if ( !GetOption( kOptionInteractive )  &&  GetOption( kOptionExitOnError )  &&  status != 0 )
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
		
		return status;
	}
	
}

