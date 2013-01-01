/*	==================
 *	NullSIOUXConsole.c
 *	==================
 */

#ifdef __MWERKS__

#include <console.h>


short InstallConsole( short /*fd*/ )
{
	return 0;
}

void RemoveConsole( void )
{
}

long WriteCharsToConsole( char* /*buffer*/, long /*n*/ )
{
	return 0;
}

long ReadCharsFromConsole( char* /*buffer*/, long /*n*/ )
{
	return 0;
}

char* __ttyname( long )
{
	return "";
}

#endif  // #ifdef __MWERKS__

