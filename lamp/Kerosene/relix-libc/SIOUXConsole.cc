/*
	SIOUXConsole.cc
	---------------
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

extern "C" int __read_console ( __file_handle handle, unsigned char* buffer, size_t* count );
extern "C" int __write_console( __file_handle handle, unsigned char* buffer, size_t* count );


short InstallConsole( short /*fd*/ )
{
	return 0;
}

void RemoveConsole()
{
}

int __write_console( __file_handle handle, unsigned char* buffer, size_t* count )
{
	*count = write( handle, buffer, *count );
	
	return *count >= 0 ? __no_io_error : __io_error;
}

int __read_console( __file_handle handle, unsigned char* buffer, size_t* count )
{
	*count = read( handle, buffer, *count );
	
	return + *count >  0 ? __no_io_error
	       : *count == 0 ? __io_EOF
	       :               __io_error;
}

#endif  // #if defined(__MWERKS__) && !defined(__MACH__)
