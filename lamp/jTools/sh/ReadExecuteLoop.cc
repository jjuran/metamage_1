// ==================
// ReadExecuteLoop.cc
// ==================

#include "ReadExecuteLoop.hh"

// Standard C/C++
#include <cstring>

// POSIX
#include "unistd.h"

// Io
#include "Io/TextInput.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// sh
#include "Execution.hh"


static void Prompt()
{
	if ( gInteractive )
	{
		Io::Out << "$ ";
	}
}

int ReadExecuteLoop( const Io::Handle& in, const std::vector< const char* >& params )
{
	Io::TextInputAdapter input( in );
	
	int result = 0;
	
	if ( gInteractive )
	{
		Io::Out << "Shell spawned with pid " << getpid() << "\n";
	}
	
	Prompt();
	
	while ( !input.Ended() )
	{
		if ( input.Ready() )
		{
			std::string command = input.Read();
			
			if ( command == "exit" )
			{
				return 0;
			}
			
			result = ExecuteCmdLine( command, params );
			
			Prompt();
		}
	}
	
	return result;
}

