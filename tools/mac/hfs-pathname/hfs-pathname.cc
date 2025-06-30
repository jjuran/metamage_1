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

// chars
#include "conv/mac_utf8.hh"

// mac-sys-utils
#include "mac_sys/errno_from_mac_error.hh"

// mac-file-utils
#include "mac_file/HFS_pathname.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"


#pragma exceptions off


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
	
	using mac::file::HFS_pathname;
	
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
		long got = HFS_pathname( NULL, 0, file );
		
		if ( (SInt32) got <= (SInt32) 0x80008000 )
		{
			UInt32 length_MacRoman = (UInt16) got;
			
			got = memFullErr;
			
			/*
				We allocate buffers using Memory Manager calls
				instead of malloc().  This avoids linking in the
				malloc pool runtime support code, and also saves
				memory -- since malloc() would have created a 64K
				non-relocatable block anyway.
			*/
			
			if ( Ptr buffer_MacRoman = NewPtr( length_MacRoman ) )
			{
				got = HFS_pathname( buffer_MacRoman, length_MacRoman, file );
				
				if ( got > 0 )
				{
					/*
						In the worst case, every character in each of the
						volume, directory, and filename components expands
						to three bytes.  However, there's guaranteed to be
						at least one colon character (':'), so that gives
						us two bytes of slack.  We'll use one of them to
						append the newline character.
					*/
					
					UInt32 length_UTF8 = length_MacRoman * 3;
					
					got = memFullErr;
					
					if ( Ptr buffer_UTF8 = NewPtr( length_UTF8 ) )
					{
						got = conv::utf8_from_mac( buffer_UTF8,
						                           length_UTF8,
						                           buffer_MacRoman,
						                           length_MacRoman );
						
						buffer_UTF8[ got++ ] = '\n';
						
						write( STDOUT_FILENO, buffer_UTF8, got );
						
						DisposePtr( buffer_UTF8 );
					}
				}
				
				DisposePtr( buffer_MacRoman );
			}
		}
		
		if ( got < 0 )
		{
			int errnum = errno_from_mac_error( got );
			
			more::perror( path, errnum );
			
			return FS_error;
		}
		
		return Success;
	}
}
