/*
	new_window.hh
	-------------
*/

#include "mac_app/new_window.hh"

// Mac OS
#ifdef __MC68K__
#ifndef __SEGLOAD__
#include <SegLoad.h>
#endif
#endif

// mac-sys-utils
#if TARGET_CPU_68K
#include "mac_sys/trap_address.hh"
#endif


#pragma exceptions off


namespace mac {
namespace app {

#if TARGET_CPU_68K  &&  ! TARGET_RT_MAC_CFM

using mac::sys::get_trap_address;

enum
{
	_Unimplemented = 0xA89F,
	_NewWindow     = 0xA913,
	_NewCWindow    = 0xAA45,
};

static inline
ProcPtr get_NewWindow_address()
{
	const ProcPtr unimplemented = get_trap_address( _Unimplemented );
	const ProcPtr newCWindow    = get_trap_address( _NewCWindow );
	
	return newCWindow != unimplemented ? newCWindow
	                                   : get_trap_address( _NewWindow );
}

const ProcPtr gNewWindow = get_NewWindow_address();

#else  // #if TARGET_CPU_68K  &&  ! TARGET_RT_MAC_CFM

int dummy;

#endif

}
}
