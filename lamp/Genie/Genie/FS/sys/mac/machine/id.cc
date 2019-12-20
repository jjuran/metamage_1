/*
	Genie/FS/sys/mac/machine/id.cc
	------------------------------
*/

#include "Genie/FS/sys/mac/machine/id.hh"

// Mac OS
#ifndef __APPLE__
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/trap_available.hh"

// plus
#include "plus/serialize.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"


#if TARGET_CPU_68K

short ROM85    : 0x28E;
char* ROMBase  : 0x2AE;
short SCSIFlag : 0xB22;

#else

static short ROM85;
static char* ROMBase;
static short SCSIFlag;

#endif


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	typedef unsigned long uint32_t;
	
	typedef plus::serialize_unsigned< uint32_t >::deconstruct deconstruct_id;
	
	
	const uint32_t gestaltMachineType = 'mach';
	
	
	static inline uint32_t GetMachineID()
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
		
		if ( TARGET_API_MAC_CARBON )
		{
			p7::throw_errno( ENOENT );
		}
		
		return 0;
	}
	
	static
	void sys_mac_machine_id_get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		const uint32_t id = GetMachineID();
		
		deconstruct_id::apply( result, id, binary );
	}
	
	const vfs::property_params sys_mac_machine_id_params =
	{
		4,  // fixed size,
		&sys_mac_machine_id_get,
	};
	
}
