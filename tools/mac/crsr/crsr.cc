/*
	crsr.cc
	-------
*/

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/cursor_location.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"


#define DEFAULT_THREAD_STACK_SIZE "Default thread stack size"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

int main( int argc, char** argv )
{
	if ( TARGET_API_MAC_CARBON )
	{
		write( STDERR_FILENO, STR_LEN( "crsr: Not supported in Carbon\n" ) );
		
		return 6;
	}
	
	using namespace mac::sys;
	
	if ( argc <= 1 )
	{
		const int fd = STDOUT_FILENO;
		
		Point location = get_cursor_location();
		
		const int max_len = (1 + 5 + 1) * 2;  // "-23456,-23456\n"
		
		char buffer[ max_len ];
		
		char* p = buffer;
		
		p = gear::inscribe_decimal_r( location.h, p );
		
		*p++ = ',';
		
		p = gear::inscribe_decimal_r( location.v, p );
		
		*p++ = '\n';
		
		int ok = wrote( fd, buffer, p - buffer );
		
		return ok ? 0 : 13;
	}
	
	if ( argc == 3  &&  strcmp( argv[ 1 ], "move-to" ) == 0 )
	{
		Point location;
		
		const char* arg = argv[ 2 ];
		const char* p = arg;
		
		location.h = gear::parse_decimal( &p );
		
		if ( p != arg  &&  *p++ == ',' )
		{
			arg = p;
			
			location.v = gear::parse_decimal( &p );
			
			if ( p != arg  &&  *p == '\0' )
			{
				set_cursor_location( location );
				
				return 0;
			}
		}
	}
	
	return 50;
}
