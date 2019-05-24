/*
	ams-fs.cc
	---------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// command
#include "command/get_option.hh"

// ams-common
#include "module_A4.hh"

// ams-fs
#include "Files.hh"
#include "mount.hh"
#include "Volumes.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-fs"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


enum
{
	Opt_last_byte = 255,
	
	Opt_disk,
};

static command::option options[] =
{
	{ "disk", Opt_disk, command::Param_required },
	
	NULL,
};


void* os_trap_table[] : 1 * 1024;

#define OSTRAP( Proc )  (os_trap_table[ _##Proc & 0x00FF ] = &Proc##_patch)


static
void install_FileManager()
{
	old_Open  = (Open_ProcPtr) os_trap_table[ _Open  & 0x00FF ];
	old_Close = (IO_ProcPtr  ) os_trap_table[ _Close & 0x00FF ];
	old_Read  = (IO_ProcPtr  ) os_trap_table[ _Read  & 0x00FF ];
	old_Write = (IO_ProcPtr  ) os_trap_table[ _Write & 0x00FF ];
	
	initialize();
	
	OSTRAP( Open   );  // A000
	OSTRAP( Close  );  // A001
	OSTRAP( Read   );  // A002
	OSTRAP( Write  );  // A003
	
	OSTRAP( GetVolInfo );  // A007
	OSTRAP( Create );  // A008
	
	OSTRAP( OpenRF );  // A00A
	
	OSTRAP( GetEOF   );  // A011
	
	OSTRAP( FlushVol );  // A013
	OSTRAP( GetVol   );  // A014
	
	OSTRAP( GetFPos  );  // A018
	
	OSTRAP( SetFPos  );  // A044
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
			case Opt_disk:
				try_to_mount( global_result.param );
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
	
	install_FileManager();
	
	mount_virtual_network_volume();
	
	module_A4_suspend();  // doesn't return
}
