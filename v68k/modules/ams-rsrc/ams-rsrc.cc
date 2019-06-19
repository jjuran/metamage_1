/*
	ams-rsrc.cc
	-----------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// ams-common
#include "module_A4.hh"

// ams-rsrc
#include "Resources.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-rsrc"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;

#define OSTRAP( Proc )  (os_trap_table     [ _##Proc & 0x00FF ] = &Proc##_patch)
#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)


static
void install_ResourceManager()
{
	TBTRAP( Count1Resources   );  // A80D
	TBTRAP( Get1IxResource    );  // A80E
	
	TBTRAP( Get1Resource      );  // A81F
	TBTRAP( Get1NamedResource );  // A820
	
	TBTRAP( DetachResource  );  // A992
	TBTRAP( SetResPurge     );  // A993
	TBTRAP( CurResFile      );  // A994
	
	TBTRAP( RsrcZoneInit    );  // A996
	TBTRAP( OpenResFile     );  // A997
	TBTRAP( UseResFile      );  // A998
	
	TBTRAP( SetResLoad      );  // A99B
	TBTRAP( CountResources  );  // A99C
	TBTRAP( GetIndResource  );  // A99D
	
	TBTRAP( GetResource     );  // A9A0
	TBTRAP( GetNamedResource);  // A9A1
	TBTRAP( LoadResource    );  // A9A2
	TBTRAP( ReleaseResource );  // A9A3
	TBTRAP( HomeResFile     );  // A9A4
	
	TBTRAP( SizeRsrc        );  // A9A5
	TBTRAP( GetResAttrs     );  // A9A6
	
	TBTRAP( GetResInfo      );  // A9A8
	
	TBTRAP( ChangedResource );  // A9AA
	
	TBTRAP( ResError        );  // A9AF
}

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		WARN( "no arguments allowed" );
		
		_exit( 1 );
	}
	
	install_ResourceManager();
	
	module_A4_suspend();  // doesn't return
}
