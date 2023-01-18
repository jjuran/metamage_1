/*
	mac2utf8.cc
	-----------
*/

// POSIX
#include <unistd.h>

// chars
#include "conv/mac_utf8.hh"

// more-posix
#include "more/perror.hh"


#pragma exceptions off


static ssize_t checked_read( int fd, char* buffer, size_t length )
{
	ssize_t n_read = read( fd, buffer, length );
	
	if ( n_read < 0 )
	{
		more::perror( "mac2utf8", "read" );
		
		_exit( 1 );
	}
	
	return n_read;
}

static void checked_write( int fd, const char* buffer, size_t length )
{
	ssize_t n_written = write( fd, buffer, length );
	
	if ( n_written < length )
	{
		more::perror( "mac2utf8", "write" );
		
		_exit( 2 );
	}
}

int main( int argc, char** argv )
{
	while ( true )
	{
		/*
			The MacRoman-encodable Unicode code point of greatest magnitude
			occupies three bytes in UTF-8, so a triple-size buffer covers the
			worst case.
		*/
		
		enum
		{
			mac_buffer_size  = 4096,
			utf8_buffer_size = mac_buffer_size * 3
		};
		
		char data_in [ mac_buffer_size  ];
		char data_out[ utf8_buffer_size ];
		
		const ssize_t bytes_read = checked_read( STDIN_FILENO,
		                                         data_in,
		                                         sizeof data_in );
		
		if ( bytes_read == 0 )
		{
			break;  // EOF
		}
		
		const size_t n_utf8_bytes = conv::utf8_from_mac( data_out,
		                                                 sizeof data_out,
		                                                 data_in,
		                                                 bytes_read );
		
		checked_write( STDOUT_FILENO, data_out, n_utf8_bytes );
	}
	
	return 0;
}
