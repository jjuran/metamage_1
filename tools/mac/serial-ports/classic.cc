/*
	classic.cc
	----------
*/

#include "classic.hh"

// POSIX
#include <unistd.h>

// mac-sys-utils
#include "mac_sys/is_driver_installed.hh"


#define STR_LEN( s )  ""s, (sizeof s - 1)


#ifdef __MACOS__

int list_classic_serial_ports( int verbosity )
{
	if ( mac::sys::is_driver_installed( "\p" ".AIn" ) )
	{
		if ( verbosity == 0 )
		{
			write( STDOUT_FILENO, STR_LEN( "Modem Port\n" ) );
		}
		else
		{
			write( STDOUT_FILENO, STR_LEN( "Modem Port [.AIn, .AOut]\n" ) );
		}
	}
	
	if ( mac::sys::is_driver_installed( "\p" ".BIn" ) )
	{
		if ( verbosity == 0 )
		{
			write( STDOUT_FILENO, STR_LEN( "Printer Port\n" ) );
		}
		else
		{
			write( STDOUT_FILENO, STR_LEN( "Printer Port [.BIn, .BOut]\n" ) );
		}
	}
	
	return 0;
}

#endif  // #ifdef __MACOS__
