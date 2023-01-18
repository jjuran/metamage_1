/*
	icon-editor.cc
	--------------
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


#define PROGRAM  "icon-editor"


static const char* icondata_path;

static int icondata_fd;

static off_t icondata_length;

static uint32_t icon_data[ 32 + 32 ];  // data + mask

const size_t text_buffer_width  = 32 * 2 + 1 + 1;
const size_t text_buffer_height = 32;

static short cursor_x;
static short cursor_y;

static bool dirty;


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

static void save()
{
	ssize_t n_written = pwrite( icondata_fd, icon_data, icondata_length, 0 );
	
	if ( n_written < 0 )
	{
		failure( icondata_path );
	}
	else if ( n_written != icondata_length )
	{
		failure( icondata_path, "incomplete write" );
	}
	else
	{
		dirty = false;
	}
}

static void move_cursor( short dx, short dy )
{
	const short new_x = cursor_x + dx;
	const short new_y = cursor_y + dy;
	
	if ( new_x >= 0  &&  new_x < 32 )
	{
		cursor_x = new_x;
	}
	
	if ( new_y >= 0  &&  new_y < 32 )
	{
		cursor_y = new_y;
	}
}

static void set( uint32_t* grid )
{
	uint32_t& row = grid[ cursor_y ];
	
	row |= 1 << (31 - cursor_x);
}

static void clear( uint32_t* grid )
{
	uint32_t& row = grid[ cursor_y ];
	
	row &= ~(1 << (31 - cursor_x));
}

static void toggle( uint32_t* grid )
{
	uint32_t& row = grid[ cursor_y ];
	
	row ^= 1 << (31 - cursor_x);
}

static bool on_char( char c )
{
	switch ( c )
	{
		case 'Q':  return false;
		
		case '>':  save();  break;
		
		case 0x1b:  return dirty;
		
		case 0x1c:  move_cursor( -1, 0 );  break;
		case 0x1d:  move_cursor(  1, 0 );  break;
		case 0x1e:  move_cursor( 0, -1 );  break;
		case 0x1f:  move_cursor( 0,  1 );  break;
		
		case 'x':  toggle( icon_data      );  dirty = true;  break;
		case '-':  toggle( icon_data + 32 );  dirty = true;  break;
		
		case '*':
			// maskless ink
			set  ( icon_data      );
			clear( icon_data + 32 );
			
			dirty = true;
			break;
		
		case 'c':
			// clear
			clear( icon_data      );
			clear( icon_data + 32 );
			
			dirty = true;
			break;
		
		case 'v':
			// white
			clear( icon_data      );
			set  ( icon_data + 32 );
			
			dirty = true;
			break;
		
		case 'b':
			// black
			set( icon_data      );
			set( icon_data + 32 );
			
			dirty = true;
			break;
		
		default:
			break;
	}
	
	return true;
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
			*p++ = ' ';
			*p++ = row & (1 << j) ? 'X' : ' ';
		}
		
		*p++ = ' ';
		*p++ = '\n';
	}
	
	if ( length == 256 )
	{
		p = text_buffer;
		
		for ( int i = 0;  i < 32;  ++i )
		{
			const uint32_t big_row = icon_data[ 32 + i ];
			
			if ( ~big_row )
			{
				++p;
				
				const uint32_t row = iota::u32_from_big( big_row );
				
				for ( int j = 31;  j >= 0;  --j )
				{
					if ( !(row & (1 << j)) )
					{
						*p = *p == ' ' ? '-' : '*';
					}
					
					p += 2;
				}
				
				++p;
			}
			else
			{
				p += text_buffer_width;
			}
		}
	}
	
	p = text_buffer + cursor_y * text_buffer_width + cursor_x * 2;
	
	p[ 0 ] = '[';
	p[ 2 ] = ']';
	
	pwrite( STDOUT_FILENO, text_buffer, sizeof text_buffer, 0 );
}

static ssize_t next_event()
{
	typedef signed char int8_t;
	
	unsigned char buffer[ 256 ];
	
	ssize_t n_read = read( STDIN_FILENO, buffer, sizeof buffer );
	
	ssize_t len = buffer[ 0 ];
	
	unsigned char* data = buffer + 1;
	
	if ( 1 + len != n_read )
	{
		return -2;
	}
	
	switch ( len )
	{
		default:
		case 0:
			break;
		
		case 1:
			if ( int8_t( *data ) >= 0 )
			{
				if ( !on_char( *data ) )
				{
					return 0;
				}
			}
			
			break;
	}
	
	return 1;
}

int main( int argc, char** argv )
{
	char* const* args = argv + (argc != 0);
	
	const char* path = args[ 0 ];
	
	if ( path == NULL )
	{
		path = "/dev/fd/0";
	}
	
	icondata_fd = open( path, O_RDWR );
	
	if ( icondata_fd < 0 )
	{
		return failure( path );
	}
	
	struct stat st;
	
	int nok = fstat( icondata_fd, &st );
	
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
	
	ssize_t n_read = read( icondata_fd, icon_data, length );
	
	if ( n_read < 0 )
	{
		return failure( path );
	}
	
	if ( n_read < sizeof length )
	{
		return failure( path, "incomplete read" );
	}
	
	icondata_path   = path;
	icondata_length = length;
	
	do
	{
		print_icon( icon_data, length );
	}
	while ( next_event() > 0 );
	
	return 0;
}
