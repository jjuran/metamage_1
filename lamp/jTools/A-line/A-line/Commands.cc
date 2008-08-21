/*	===========
 *	Commands.cc
 *	===========
 */

#include "A-line/Commands.hh"

// A-line
#include "A-line/A-line.hh"


namespace ALine
{
	
	static void Echo( const char* string )
	{
		Command echo;
		
		echo.push_back( "/bin/echo" );
		echo.push_back( string );
		echo.push_back( NULL );
		
		ExecuteCommand( echo );
	}
	
	
	void CommandTask::Main()
	{
		Echo( itsCaption.c_str() );
		
		ExecuteCommand( itsCommand, itsDiagnosticsFile.c_str() );
	}
	
}

