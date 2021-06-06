// ==================
// ReadExecuteLoop.cc
// ==================

#include "ReadExecuteLoop.hh"

// Standard C/C++
#include <cstring>

// Standard C
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <sys/ioctl.h>

// gear
#include "gear/find.hh"
#include "gear/inscribe_decimal.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
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
	
	static inline
	void assign_shell_variable( const char* name, int value )
	{
		AssignShellVariable( name, gear::inscribe_decimal( value ) );
	}
	
	static void SetRowsAndColumns()
	{
		int fd = open( "/dev/tty", O_RDONLY );
		
		if ( fd >= 0 )
		{
			struct winsize size = { 0 };
			
			int nok = ioctl( fd, TIOCGWINSZ, &size );
			
			if ( nok == 0 )
			{
				assign_shell_variable( "LINES",   size.ws_row );
				assign_shell_variable( "COLUMNS", size.ws_col );
			}
			
			close( fd );
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
		
		const unsigned char whitespace[] = { 2, ' ', '\t' };
		
		while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
		{
			// Only process non-blank lines
			if ( gear::find_first_nonmatch( s->data(), s->size(), whitespace ) )
			{
				plus::string command( *s );
				
				{
					SetRowsAndColumns();
					
					status = ExecuteCmdLine( command );
					
					if ( !GetOption( kOptionInteractive )  &&  GetOption( kOptionExitOnError )  &&  status != 0 )
					{
						break;
					}
				}
			}
			
			if ( prompts )
			{
				SendPrompt();
			}
		}
		
		return status;
	}
	
}
