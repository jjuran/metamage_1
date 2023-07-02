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
typedef unsigned long	file_handle;

enum io_results
{
     no_io_error,
     io_error,
     io_EOF
};

extern "C" int __read_console ( file_handle handle, unsigned char* buffer, size_t* count );
extern "C" int __write_console( file_handle handle, unsigned char* buffer, size_t* count );


short InstallConsole( short /*fd*/ )
{
	return 0;
}

void RemoveConsole()
{
}

int __write_console( file_handle handle, unsigned char* buffer, size_t* count )
{
	*count = write( handle, buffer, *count );
	
	return *count >= 0 ? no_io_error : io_error;
}

int __read_console( file_handle handle, unsigned char* buffer, size_t* count )
{
	*count = read( handle, buffer, *count );
	
	return + *count >  0 ? no_io_error
	       : *count == 0 ? io_EOF
	       :               io_error;
}

#endif  // #if defined(__MWERKS__) && !defined(__MACH__)
