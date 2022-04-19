/*
	mac-id.cc
	---------
*/

// Mac OS
#ifndef __APPLE__
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/trap_available.hh"

// gear
#include "gear/inscribe_decimal.hh"


#if TARGET_CPU_68K

short ROM85    : 0x28E;
char* ROMBase  : 0x2AE;
short SCSIFlag : 0xB22;

#else

static short ROM85;
static char* ROMBase;
static short SCSIFlag;

#endif

enum
{
	gestaltMachineType = 'mach',
};

static
long get_machine_id()
{
	if ( const long id = mac::sys::gestalt( gestaltMachineType ) )
	{
		return id;
	}
	
#if TARGET_CPU_68K
	
	enum
	{
		_CountADBs   = 0xA077,
		_SysEnvirons = 0xA090,
		
		gestaltClassic  = 1,
		gestaltMacXL    = 2,
		gestaltMac512KE = 3,
		gestaltMacPlus  = 4,
		gestaltMacSE    = 5,
	};
	
	using mac::sys::trap_available;
	
	if ( trap_available( _SysEnvirons ) )
	{
		SysEnvRec env;
		
		SysEnvirons( curSysEnvVers, &env );
		
		return env.machineType + 2;
	}
	
	return + trap_available( _CountADBs ) ? gestaltMacSE
	       : SCSIFlag < 0                 ? gestaltMacPlus
	       : ROM85 > 0                    ? gestaltMac512KE
	       : ROMBase[ 8 ] != 0x00         ? gestaltMacXL
	       :                                gestaltClassic;
	
#endif
	
	return 0;
}

int main( int argc, char** argv )
{
	long id = get_machine_id();
	
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
