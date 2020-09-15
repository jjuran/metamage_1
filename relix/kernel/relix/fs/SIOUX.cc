/*
	SIOUX.cc
	--------
*/

// Metrowerks Standard Library
#ifdef __MWERKS__
//#include <console.h>
#endif

// relix
#include "relix/fs/console.hh"

/*
	Declare the functions here so we can override __ttyname() to return
	const char* instead of char*.
	
	For non-Metrowerks contexts:
	  * Declare these functions in a namespace to avoid potential conflicts.
	  * The symbol __ttyname is reserved, so use ttyname instead.
*/

#ifdef __MWERKS__
#define ttyname __ttyname
extern "C" {
#else
namespace dummy {
#endif

short InstallConsole( short fd );
void RemoveConsole();
long WriteCharsToConsole ( char *buffer, long n );
long ReadCharsFromConsole( char *buffer, long n );
const char* ttyname( long fd );

short InstallConsole( short fd )
{
	return 0;
}

void RemoveConsole()
{
}

long WriteCharsToConsole( char* buffer, long n )
{
	return relix::console::log( buffer, n );
}

long ReadCharsFromConsole( char* buffer, long n )
{
	return 0;
}

const char* ttyname( long fd )
{
	return "/dev/console";
}

}  // extern "C" OR namespace dummy
