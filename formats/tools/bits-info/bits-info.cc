/*
	bits-info.cc
	------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdint.h>

// more-posix
#include "more/perror.hh"

// bits
#include "bits/bits.hh"


#define PROGRAM  "bits-info"


static bits::header bits_header;


static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
}

static
char* inscribe_backwards( unsigned short x, char* p )
{
	enum
	{
		base = 10,
	};
	
	while ( x )
	{
		*--p = '0' + x % base;
		
		x /= base;
	}
	
	return p;
}

static
int open_bits( const char* path )
{
	int fd = open( path, O_RDONLY );
	
	if ( fd < 0 )
	{
		report_error( path, errno );
	}
	else
	{
		ssize_t n_read = read( fd, &bits_header, sizeof bits_header );
		
		int errnum = errno;
		
		close( fd );
		
		if ( n_read < 0 )
		{
			report_error( path, errnum );
		}
		else if ( n_read != sizeof bits_header )
		{
			report_error( path, ENOSPC );  // our convention for a short read
		}
		else
		{
			return 0;
		}
	}
	
	return -1;
}

static
void print_info()
{
	char buffer[ sizeof "12345x54321 (12345 B)" ];
	
	char* q = buffer + sizeof buffer;
	char* p = buffer + sizeof buffer;
	
	*--p = '\n';
	*--p = ')';
	*--p = 'B';
	*--p = ' ';
	
	uint16_t width  = bits_header.width;
	uint16_t height = bits_header.height;
	uint16_t stride = bits_header.stride;
	
	p = inscribe_backwards( stride, p );
	
	*--p = '(';
	*--p = ' ';
	
	p = inscribe_backwards( height, p );
	
	*--p = 'x';
	
	p = inscribe_backwards( width, p );
	
	write( STDOUT_FILENO, p, q - p );
}

int main( int argc, char** argv )
{
	if ( *argv++ == NULL )
	{
		return 0;
	}
	
	int errors = 0;
	
	while ( const char* path = *argv++ )
	{
		int nok = open_bits( path );
		
		errors |= nok;
		
		if ( ! nok )
		{
			print_info();
		}
	}
	
	return errors ? 1 : 0;
}
