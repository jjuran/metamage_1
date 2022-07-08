/*
	mac-id.cc
	---------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/get_machine_id.hh"

// gear
#include "gear/inscribe_decimal.hh"


int main( int argc, char** argv )
{
	long id = mac::sys::get_machine_id();
	
	if ( id == 0 )
	{
		return 1;
	}
	
	char* decimal = gear::inscribe_unsigned_decimal( id );
	
	size_t len = strlen( decimal );
	
	decimal[ len++ ] = '\n';
	
	ssize_t n = write( STDOUT_FILENO, decimal, len );
	
	if ( n != len )
	{
		return 13;
	}
	
	return 0;
}
