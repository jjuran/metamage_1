/*
	utf82mac.cc
	-----------
*/

// POSIX
#include <unistd.h>

// chars
#include "conv/mac_utf8.hh"

// more-posix
#include "more/perror.hh"


#pragma exceptions off


#define PROGRAM "utf82mac"

static
void error( const char* msg, int exit_status )
{
	more::perror( PROGRAM, msg );
	
	_exit( exit_status );
}

static ssize_t checked_read( int fd, char* buffer, size_t length )
{
	ssize_t n_read = read( fd, buffer, length );
	
	if ( n_read < 0 )
	{
		more::perror( "utf82mac", "read" );
		
		_exit( 1 );
	}
	
	return n_read;
}

static void checked_write( int fd, const char* buffer, size_t length )
{
	ssize_t n_written = write( fd, buffer, length );
	
	if ( n_written < length )
	{
		more::perror( "utf82mac", "write" );
		
		_exit( 2 );
	}
}

int main( int argc, char** argv )
{
	while ( true )
	{
		/*
			MacRoman is an extended-ASCII character set (having 256 code points)
			so Unicode code points map to a single byte value or nothing at all.
			Therefore the buffer need be no longer than the UTF-8 input buffer.
		*/
		
		enum
		{
			utf8_buffer_size = 4096,
			mac_buffer_size  = utf8_buffer_size
		};
		
		char data_in [ utf8_buffer_size ];
		char data_out[ mac_buffer_size  ];
		
		const ssize_t bytes_read = checked_read( STDIN_FILENO,
		                                         data_in,
		                                         sizeof data_in );
		
		if ( bytes_read == 0 )
		{
			break;  // EOF
		}
		
		const size_t n_mac_bytes = conv::mac_from_utf8_nothrow( data_out,
		                                                        sizeof data_out,
		                                                        data_in,
		                                                        bytes_read );
		
		switch ( n_mac_bytes )
		{
			case conv::invalid_utf8:  error( "UTF-8 decoding error", 3 );
			case conv::non_Mac_utf8:  error( "Unrepresentable character", 4 );
			
			default:
				break;
		}
		
		checked_write( STDOUT_FILENO, data_out, n_mac_bytes );
	}
	
	return 0;
}
