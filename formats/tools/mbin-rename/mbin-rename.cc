/*
	mbin-rename.cc
	--------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdio.h>
#include <string.h>

// chars
#include "conv/mac_utf8.hh"

// macbinary
#include "macbinary.hh"


#pragma exceptions off


#define STR_LEN(s)  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( msg "\n" ) )


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
	if ( argc != 3 )
	{
		WARN( "usage: mbin-rename file.mbin NewName" );
		
		return Usage_error;
	}
	
	const char* input_path = argv[ 1 ];
	const char* name_UTF_8 = argv[ 2 ];
	
	int fd = open( input_path, O_RDWR );
	
	if ( fd < 0 )
	{
		perror( input_path );
		return FS_error;
	}
	
	int buffer[ 512 / sizeof (int) ];  // ensure alignment
	
	ssize_t n_read = read( fd, buffer, sizeof buffer );
	
	if ( n_read < 0 )
	{
		perror( input_path );
		return IO_error;
	}
	
	if ( n_read >= sizeof (macbinary::header) )
	{
		macbinary::header& h = *(macbinary::header*) buffer;
		
		if ( int8_t v = version( h ) )
		{
			using conv::mac_from_utf8_nothrow;
			
			uint8_t* name = h.filename;
			
			memset( name, '\0', 64 );  // clobber entire name field
			
			char* p = (char*) name + 1;
			
			const size_t len = strlen( name_UTF_8 );
			
			size_t n = mac_from_utf8_nothrow( p, 32, name_UTF_8, len );
			
			if ( n > 31 )
			{
				fprintf( stderr, "n: %ld\n", (long) n );
				
				WARN( "mbin-rename: invalid name" );
				
				return Usage_error;
			}
			
			*name++ = n;
			
			update_crc( h );
			
			if ( ! version( h ) )
			{
				WARN( "mbin-rename: invalid checksum!" );
				
				return 13;
			}
			
			ssize_t n_written = pwrite( fd, buffer, sizeof buffer, 0 );
			
			if ( n_written < 0 )
			{
				perror( input_path );
				return IO_error;
			}
			
			return Success;
		}
	}
	
	fprintf( stderr, "%s: not MacBinary\n", input_path );
	
	return Failure;
}
