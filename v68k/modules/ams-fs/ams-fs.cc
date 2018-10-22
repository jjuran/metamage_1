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

// ams-common
#include "module_A4.hh"

// ams-fs
#include "Files.hh"
#include "Volumes.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-fs"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


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
	
	OSTRAP( Create );  // A008
	
	OSTRAP( OpenRF );  // A00A
	
	OSTRAP( FlushVol );  // A013
	OSTRAP( GetVol   );  // A014
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = ++argv;
		
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
