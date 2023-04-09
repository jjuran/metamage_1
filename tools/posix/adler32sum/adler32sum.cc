/*
	adler32sum.cc
	-------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <string.h>

// more-posix
#include "more/perror.hh"

// gear
#include "gear/hexadecimal.hh"

// adler32
#include "adler32/adler32.hh"


#pragma exceptions off


#define PROGRAM  "adler32sum"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static inline
void report_error( const char* path )
{
	more::perror( PROGRAM, path );
}

static
int adler32sum( const char* path )
{
	const bool input_is_default = strcmp( path, "-" ) == 0;
	
	const int input_fd = input_is_default ? 0 : open( path, O_RDONLY );
	
	if ( input_fd < 0 )
	{
		report_error( path );
		
		return 1;
	}
	
	const size_t buffer_size = 4096;
	
	char buffer[ 4096 ];
	
	unsigned checksum;
	
	// loop exits on a partial block or at EOF
	while ( true )
	{
		ssize_t n_read = read( input_fd, buffer, sizeof buffer );
		
		if ( n_read < 0 )
		{
			report_error( path );
			
			return 1;
		}
		
		checksum = adler32::checksum( buffer, n_read );
		break;
	}
	
	char checksum_buffer[ 8 ];
	
	gear::encode_32_bit_hex( checksum, checksum_buffer );
	
	write( STDOUT_FILENO, checksum_buffer, sizeof checksum_buffer );
	
	if ( ! input_is_default )
	{
		write( STDOUT_FILENO, STR_LEN( "  " ) );
		
		write( STDOUT_FILENO, path, strlen( path ) );
	}
	
	write( STDOUT_FILENO, STR_LEN( "\n" ) );
	
	return 0;
}

int main( int argc, char** argv )
{
	bool had_errors = false;
	
	for ( int i = 1;  i < argc;  ++i )
	{
		had_errors = adler32sum( argv[ i ] )  ||  had_errors;
	}
	
	return had_errors ? 1 : 0;
}
