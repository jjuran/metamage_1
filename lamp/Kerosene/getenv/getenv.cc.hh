/*
	getenv.cc.hh
	------------
*/

// Standard C
#include <string.h>


// getenv() must already be declared

extern "C" char** environ;


char* getenv( const char* name )
{
	if ( environ != NULL )
	{
		for ( char** env = environ;  *env != NULL;  ++env )
		{
			const char* var = *env;
			
			if ( char* eq = strchr( var, '=' ) )
			{
				const size_t name_length = eq - var;
				
				if ( memcmp( name, var, name_length ) == 0  &&  name[ name_length ] == '\0' )
				{
					return eq + 1;
				}
			}
		}
	}
	
	return NULL;
}

