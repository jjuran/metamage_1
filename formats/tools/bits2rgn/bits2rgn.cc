/*
	bits2rgn.cc
	-----------
*/

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// more-libc
#include "more/string.h"

// more-posix
#include "more/perror.hh"

// bits
#include "bits/bits.hh"


#pragma exceptions off


#define PROGRAM  "bits2rgn"

#define MISSING_INPUT   "input file required"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static inline
void report_error( const char* path, unsigned err )
{
	more::perror( PROGRAM, path, err );
}

static inline
RgnHandle region_from_BitMap( const BitMap& bits )
{
	if ( RgnHandle rgn = NewRgn() )
	{
		OSErr err = BitMapToRegion( rgn, &bits );
		
		if ( err == noErr )
		{
			return rgn;
		}
		
		DisposeRgn( rgn );
	}
	
	return NULL;
}

static inline
RgnHandle bits_to_region( const char* path )
{
	int fd = open( path, O_RDONLY );
	
	if ( fd < 0 )
	{
		report_error( path, errno );
		
		return NULL;
	}
	
	RgnHandle rgn = NULL;
	
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	size_t size = st.st_size;
	
	if ( nok  ||  (! S_ISREG( st.st_mode )  &&  (errno = ESPIPE)) )
	{
		report_error( path, errno );
	}
	else if ( void* buffer = malloc( size ) )
	{
		ssize_t n = read( fd, buffer, size );
		
		const bits::header& bits = *(const bits::header*) buffer;
		
		if ( n < 0 )
		{
			report_error( path, errno );
		}
		else if ( n != size )
		{
			report_error( path, ENOSPC );
		}
		else if ( ! valid( bits, size ) )
		{
			report_error( path, EINVAL );
		}
		else
		{
			const int stride = bits.stride;
			const int height = bits.height;
			const int width  = bits.width;
			
			Ptr p = (Ptr) buffer + sizeof (::bits::header);
			
			BitMap bitmap =
			{
				p,
				stride,
				{ 0, 0, height, width },
			};
			
			rgn = region_from_BitMap( bitmap );
		}
		
		free( buffer );
	}
	else
	{
		report_error( "malloc", errno );
	}
	
	close( fd );
	
	return rgn;
}

int main( int argc, char** argv )
{
	const char* output_path = NULL;
	
	char** args = argv + 1;
	
	const char* src = *args;
	
	if ( src == NULL )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " MISSING_INPUT "\n" ) );
		return 2;
	}
	
	if ( argc >= 3  &&  strcmp( *args, "-o" ) == 0 )
	{
		++args;
		output_path = *args++;
		
		src = *args;
	}
	else
	{
		size_t len = strlen( src );
		
		void* p = alloca( len + sizeof ".RGN" );
		
		output_path = (char*) p;
		
		p = mempcpy( p, src,    len           );
		p = mempcpy( p, ".RGN", sizeof ".RGN" );  // includes final NUL byte
	}
	
	RgnHandle rgn = bits_to_region( src );
	
	if ( ! rgn )
	{
		report_error( "bits_to_region", errno );
		return 1;
	}
	
	HLock( (Handle) rgn );
	
	Size size = rgn[0]->rgnSize;
	
	ssize_t wrote = 0;
	
	int write_errnum = 0;
	
	int closed = 0;
	
	int fd = open( output_path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	
	if ( fd >= 0 )
	{
		wrote = write( fd, *rgn, size );
		
		write_errnum = errno;
		
		closed = close( fd );
	}
	
	DisposeRgn( rgn );
	
	int errnum = fd < 0        ? errno
	           : wrote < 0     ? write_errnum
	           : wrote != size ? ENOSPC
	           : closed < 0    ? errno
	           :                 0;
	
	if ( errnum )
	{
		report_error( output_path, errnum );
		
		return 1;
	}
	
	return 0;
}
