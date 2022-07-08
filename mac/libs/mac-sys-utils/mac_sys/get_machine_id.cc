/*
	get_machine_id.cc
	-----------------
*/

#include "mac_sys/get_machine_id.hh"

// Mac OS
#ifndef __APPLE__
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/trap_available.hh"


namespace mac {
namespace sys {

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

long get_machine_id()
{
	if ( const long id = gestalt( gestaltMachineType ) )
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
	
}
}
