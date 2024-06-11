/*
	ams-io.cc
	---------
*/

// POSIX
#include <unistd.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// ams-common
#include "callouts.hh"

// ams-io
#include "Devices.hh"
#include "options.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-io"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


#ifndef __TRAPS__

enum
{
	_Open    = 0xA000,
	_Close   = 0xA001,
	_Read    = 0xA002,
	_Write   = 0xA003,
	_Control = 0xA004,
	_Status  = 0xA005,
	_KillIO  = 0xA006,
};

#endif

enum
{
	Opt_last_byte = 255,
	
	Opt_modem_fd,
};

static command::option options[] =
{
	{ "modem-fd", Opt_modem_fd, command::Param_required },
	
	NULL,
};


void* os_trap_table[] : 1 * 1024;

#define OSTRAP( Proc )  (os_trap_table[ _##Proc & 0x00FF ] = &Proc##_patch)


static
void install_DeviceManager()
{
	OSTRAP( Open    );  // A000
	OSTRAP( Close   );  // A001
	OSTRAP( Read    );  // A002
	OSTRAP( Write   );  // A003
	OSTRAP( Control );  // A004
	OSTRAP( Status  );  // A005
	OSTRAP( KillIO  );  // A006
}

static
char* const* get_options( char** argv )
{
	using command::global_result;
	
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_modem_fd:
				modem_fd = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = get_options( argv );
		
		if ( *args != NULL )
		{
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	install_DeviceManager();
	
	install_drivers();
	
	module_suspend();  // doesn't return
}
