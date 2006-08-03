// ==================
// ReadExecuteLoop.cc
// ==================

#include "ReadExecuteLoop.hh"

// Standard C
#include "stdlib.h"

// Standard C/C++
#include <cstring>

// POSIX
#include "unistd.h"

// Io
#include "Io/MakeHandle.hh"
#include "Io/TextInput.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// sh
#include "Execution.hh"


static void Prompt()
{
	const char* ps1 = getenv( "PS1" );
	
	if ( ps1 == NULL )
	{
		ps1 = "$ ";
	}
	
	Io::Out << ps1;
}

int ReadExecuteLoop( P7::FileDescriptor  fd,
                     bool                prompts )
{
	Io::Handle in = Io::MakeHandleFromCast< Io::FD_Details, Io::FD >( Io::FD( int( fd ) ) );
	
	Io::TextInputAdapter input( in );
	
	int result = 0;
	
	if ( prompts )
	{
		Io::Out << "Shell spawned with pid " << getpid() << "\n";
		
		Prompt();
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
			
			if ( prompts )
			{
				Prompt();
			}
		}
	}
	
	return result;
}

