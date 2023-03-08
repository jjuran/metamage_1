/*
	dump-rom-rsrc.cc
	----------------
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// mac-glue-utils
#include "mac_glue/Memory.hh"

// gear
#include "gear/parse_decimal.hh"


char ROMMapInsert : 0x0B9E;


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "dump-rom-rsrc"
#define USAGE    "Usage: " PROGRAM ": <res-type> <res-id>"
#define ERROR    PROGRAM ": error getting resource"
#define ISATTY   PROGRAM ": not writing to tty (redirect output instead)"


int main( int argc, char** argv )
{
	if ( argc != 3 )
	{
		goto usage;
	}
	
	const char* res_type = argv[ 1 ];
	const char* res_id   = argv[ 2 ];
	
	if ( strlen( res_type ) != 4 )
	{
		goto usage;
	}
	
	const int id = gear::parse_decimal( res_id );
	
	if ( (short) id != id )
	{
		goto usage;
	}
	
	const ResType type = *(const ResType*) res_type;
	
	ROMMapInsert = 0xFF;
	
	Handle h = GetResource( type, id );
	
	if ( h == 0 )
	{
		write( STDERR_FILENO, STR_LEN( ERROR "\n" ) );
		return 1;
	}
	
	if ( isatty( STDOUT_FILENO ) )
	{
		write( STDERR_FILENO, STR_LEN( ISATTY "\n" ) );
		return 0;
	}
	
	Size size = mac::glue::GetHandleSize( h );
	
	write( STDOUT_FILENO, *h, size );
	
	return 0;
	
usage:
	write( STDERR_FILENO, STR_LEN( USAGE "\n" ) );
	return 2;
}
