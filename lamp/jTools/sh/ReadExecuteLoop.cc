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

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/ioctl.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/write.hh"

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
			
			AssignShellVariable( "LINES",   iota::inscribe_decimal( size.ws_row ) );
			AssignShellVariable( "COLUMNS", iota::inscribe_decimal( size.ws_col ) );
		}
		catch ( ... )
		{
		}
	}
	
	p7::wait_t ReadExecuteLoop( p7::fd_t  fd,
	                            bool      prompts )
	{
		p7::wait_t status = p7::wait_t( 0 );
		
		if ( prompts )
		{
			SendPrompt();
		}
		
		text_input::feed feed;
		
		p7::fd_reader reader( fd );
		
		while ( const plus::string* s = get_line_from_feed( feed, reader ) )
		{
			{
				plus::string command( s->begin(), s->end() - 1 );
				
				// Only process non-blank lines
				if ( command.find_first_not_of( " \t" ) != command.npos )
				{
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

