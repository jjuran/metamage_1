/*
	ams-fs.cc
	---------
*/

// POSIX
#include <unistd.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-fs"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


static
asm void module_suspend()
{
	JSR      0xFFFFFFF8
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = ++argv;
		
		if ( *args != NULL )
		{
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	module_suspend();  // doesn't return
}
