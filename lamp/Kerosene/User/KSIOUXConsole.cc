/*	================
 *	KSIOUXConsole.cc
 *	================
 */

#if defined(__MWERKS__) && !defined(__MACH__)

// Metrowerks SIOUX console stubs
#include <console.h>

// POSIX
#include "unistd.h"


#pragma exceptions off


// from file_struc.h
typedef unsigned long	__file_handle;

enum __io_results 
{
     __no_io_error,
     __io_error,
     __io_EOF
};

extern "C" int __write_console( __file_handle handle, unsigned char* buffer, size_t* count );


enum
{
	kConsoleInputFileDescriptor  = 0,
	kConsoleOutputFileDescriptor = 1
};


short InstallConsole( short /*fd*/ )
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

int __write_console( __file_handle handle, unsigned char* buffer, size_t* count )
{
	*count = write( handle, buffer, *count );
	
	return *count >= 0 ? __no_io_error : __io_error;
}

#endif  // #if defined(__MWERKS__) && !defined(__MACH__)

