/*	================
 *	KSIOUXConsole.cc
 *	================
 */

#if defined(__MWERKS__) && !defined(__MACH__)

// Metrowerks SIOUX console stubs
#include <console.h>

// POSIX
#include "unistd.h"


enum
{
	kConsoleInputFileDescriptor  = 0,
	kConsoleOutputFileDescriptor = 1
};


short InstallConsole( short fd )
{
	return 0;
}

void RemoveConsole()
{
}

long WriteCharsToConsole( char *buffer, long n )
{
	return write( kConsoleOutputFileDescriptor, buffer, n );
}

long ReadCharsFromConsole( char *buffer, long n )
{
	return read( kConsoleInputFileDescriptor, buffer, n );
}

#endif  // #if defined(__MWERKS__) && !defined(__MACH__)

