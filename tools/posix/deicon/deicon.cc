/*
	deicon.cc
	---------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <stdint.h>

// iota
#include "iota/endian.hh"

// more-posix
#include "more/perror.hh"


#pragma exceptions off


#define PROGRAM  "deicon"


const size_t text_buffer_width  = 32 * 2;
const size_t text_buffer_height = 32;


static inline int failure( const char* s )
{
	more::perror( PROGRAM, s );
	
	return 1;
}

static inline int failure( const char* s1, const char* s2 )
{
	more::perror( PROGRAM, s1, s2 );
	
	return 1;
}

static void print_icon( const uint32_t* icon_data, off_t length )
{
	char text_buffer[ text_buffer_width * text_buffer_height ];
	
	char* p = text_buffer;
	
	for ( int i = 0;  i < 32;  ++i )
	{
		const uint32_t row = iota::u32_from_big( icon_data[ i ] );
		
		for ( int j = 31;  j >= 0;  --j )
		{
			*p++ = row & (1 << j) ? 'X' : ' ';
			*p++ = ' ';
		}
		
		p[ -1 ] = '\n';
	}
	
	if ( length == 256 )
	{
		p = text_buffer;
		
		for ( int i = 0;  i < 32;  ++i )
		{
			const uint32_t big_row = icon_data[ 32 + i ];
			
			if ( ~big_row )
			{
				const uint32_t row = iota::u32_from_big( big_row );
				
				for ( int j = 31;  j >= 0;  --j )
				{
					if ( !(row & (1 << j)) )
					{
						*p = *p == ' ' ? '-' : '*';
					}
					
					p += 2;
				}
			}
			else
			{
				p += text_buffer_width;
			}
		}
	}
	
	write( STDOUT_FILENO, text_buffer, sizeof text_buffer );
}

int main( int argc, char** argv )
{
	char* const* args = argv + (argc != 0);
	
	const char* path = args[ 0 ];
	
	if ( path == NULL )
	{
		path = "/dev/fd/0";
	}
	
	int fd = open( path, O_RDONLY );
	
	if ( fd < 0 )
	{
		return failure( path );
	}
	
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	if ( nok )
	{
		return failure( path );
	}
	
	const off_t length = st.st_size;
	
	if ( !S_ISREG( st.st_mode ) )
	{
		return failure( path, "regular file expected" );
	}
	
	if ( length != 128  &&  length != 256 )
	{
		return failure( path, "unrecognized icon length" );
	}
	
	uint32_t icon_data[ 32 + 32 ];  // data + mask
	
	ssize_t n_read = read( fd, icon_data, length );
	
	if ( n_read < 0 )
	{
		return failure( path );
	}
	
	if ( n_read < sizeof length )
	{
		return failure( path, "incomplete read" );
	}
	
	print_icon( icon_data, length );
	
	return 0;
}
