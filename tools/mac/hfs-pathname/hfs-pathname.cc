/*
	hfs-pathname.cc
	---------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// POSIX
#include <unistd.h>

// more-posix
#include "more/perror.hh"

// mac-sys-utils
#include "mac_sys/errno_from_mac_error.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"

// GetPathname
#include "GetMacPathname.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "hfs-pathname"
#define USAGE    "Usage: " PROGRAM " <path>"


enum
{
	Success,
	Failure,
	
	FS_error = 30,
	IO_error = 40,
	
	Usage_error = 50,  // mnemonic:  Mac OS paramErr = -50
};

int main( int argc, char** argv )
{
	if ( argc != 2 )
	{
		write( STDERR_FILENO, STR_LEN( USAGE "\n" ) );
		
		return Usage_error;
	}
	
	using mac::relix::FSSpec_from_existing_path;
	
	using mac::sys::Error;
	using mac::sys::errno_from_mac_error;
	
	const char* path = argv[ 1 ];
	
	FSSpec file;
	
	if ( Error err = FSSpec_from_existing_path( path, file ) )
	{
		int errnum = is_errno( err ) ? errno_from_muxed( err )
		                             : errno_from_mac_error( err );
		
		more::perror( path, errnum );
		
		return FS_error;
	}
	else
	{
		plus::string mac_path = GetMacPathname( file, true );
		
		size_t n = mac_path.size();
		
		const char* data = mac_path.data();
		
		((char*) data)[ n++ ] = '\n';
		
		write( STDOUT_FILENO, data, n );
		
		return Success;
	}
}
