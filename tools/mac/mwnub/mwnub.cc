/*
	mwnub.cc
	--------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// mwdebug
#include "mwdebug/api.hh"


#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define VERSION  "Version: "
#define STATUS   "Status:  "


using namespace mwdebug;


static const char* state_names[] =
{
	"OFFLINE",
	"OBSOLETE",
	"INSTALLED",
	"RUNNING",
	"ATTACHED",
};

/*
	Output:
	
		Version: 1.2.3
		Status:  RUNNING
*/

const int max_output_size = STRLEN( VERSION ) + 31 + 1
                          + STRLEN( STATUS  ) + STRLEN( "INSTALLED" ) + 1;

int main( int argc, char** argv )
{
	int status = 0;
	
	if ( const unsigned char* nubVersion = MetroNubVersion() )
	{
		++status;
		
		if ( IsMetroNubInstalled() )
		{
			++status;
			
			if ( IsDebuggerRunning() )
			{
				++status;
				
				if ( AmIBeingDebugged() )
				{
					++status;
				}
			}
		}
		
		const char* state = state_names[ status ];
		
		const size_t length = nubVersion[ 0 ];
		
		char buffer[ max_output_size ] = VERSION;
		
		char* p = buffer + STRLEN( VERSION );
		
		p = (char*) mempcpy( p, &nubVersion[ 1 ], length );
		
		p = (char*) mempcpy( p, STR_LEN( "\n" STATUS ) );
		
		p = (char*) mempcpy( p, state, strlen( state ) );
		
		*p++ = '\n';
		
		int err = write( STDOUT_FILENO, buffer, p - buffer );
		
		if ( err )
		{
			return 2;
		}
		
		return 0;
	}
	
	return 1;
}
