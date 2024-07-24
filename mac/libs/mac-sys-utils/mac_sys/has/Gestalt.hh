/*
	Gestalt.hh
	----------
*/

#ifndef MACSYS_HAS_GESTALT_HH
#define MACSYS_HAS_GESTALT_HH

// Mac OS
#ifndef __TARGETCONDITIONALS__
#include <TargetConditionals.h>
#endif


#define mac_sys_LOWMEM( addr, type )  (*(type*) (addr))

#define mac_sys_SysVersion  mac_sys_LOWMEM( 0x015A, short )


namespace mac {
namespace sys {

inline
bool has_Gestalt()
{
	return ! TARGET_CPU_68K  ||  mac_sys_SysVersion >= 0x0604;
}

}
}

#endif
