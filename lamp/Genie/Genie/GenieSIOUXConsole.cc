/*	====================
 *	GenieSIOUXConsole.cc
 *	====================
 */

#ifdef __MWERKS__

// Metrowerks Standard Library
#include <console.h>

// relix
#include "relix/fs/console.hh"


short InstallConsole( short /*fd*/ )
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

long ReadCharsFromConsole( char* /*buffer*/, long /*n*/ )
{
	return 0;
}

char* __ttyname( long )
{
	return "/dev/console";
}

#endif  // #ifdef __MWERKS__
