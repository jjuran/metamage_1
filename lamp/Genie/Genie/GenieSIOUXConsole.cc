/*	====================
 *	GenieSIOUXConsole.cc
 *	====================
 */

#ifdef __MWERKS__

// Metrowerks Standard Library
#include <console.h>

// Genie
#include "Genie/SystemConsole.hh"


short InstallConsole( short /*fd*/ )
{
	return 0;
}

void RemoveConsole()
{
}

long WriteCharsToConsole( char* buffer, long n )
{
	return Genie::WriteToSystemConsole( buffer, n );
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

