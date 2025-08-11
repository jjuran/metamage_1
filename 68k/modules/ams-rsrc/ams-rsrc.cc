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
#include "Init.hh"
#include "Resources.hh"
#include "Utilities.hh"


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
	TBTRAP( Get1IxType        );  // A80F
	TBTRAP( Unique1ID         );  // A810
	
	TBTRAP( Count1Types       );  // A81C
	
	TBTRAP( Get1Resource      );  // A81F
	TBTRAP( Get1NamedResource );  // A820
	
	TBTRAP( DetachResource  );  // A992
	TBTRAP( SetResPurge     );  // A993
	TBTRAP( CurResFile      );  // A994
	TBTRAP( InitResources   );  // A995
	TBTRAP( RsrcZoneInit    );  // A996
	TBTRAP( OpenResFile     );  // A997
	TBTRAP( UseResFile      );  // A998
	TBTRAP( UpdateResFile   );  // A999
	TBTRAP( CloseResFile    );  // A99A
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
	TBTRAP( SetResAttrs     );  // A9A7
	TBTRAP( GetResInfo      );  // A9A8
	TBTRAP( SetResInfo      );  // A9A9
	TBTRAP( ChangedResource );  // A9AA
	TBTRAP( AddResource     );  // A9AB
	
	TBTRAP( ResError        );  // A9AF
	TBTRAP( WriteResource   );  // A9B0
	TBTRAP( CreateResFile   );  // A9B1
	
	TBTRAP( OpenRFPerm      );  // A9C4
	
	TBTRAP( GetResFileAttrs );  // A9F6
}

static void install_Utilities()
{
	TBTRAP( GetPattern );  // A9B8
	TBTRAP( GetCursor  );  // A9B9
	TBTRAP( GetString  );  // A9BA
	TBTRAP( GetIcon    );  // A9BB
	TBTRAP( GetPicture );  // A9BC
}

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		WARN( "no arguments allowed" );
		
		_exit( 1 );
	}
	
	install_ResourceManager();
	
	install_Utilities();
	
	module_A4_suspend();  // doesn't return
}
