/*	=====================
 *	SystemCallRegistry.hh
 *	=====================
 */

#ifndef GENIE_SYSTEMCALLREGISTRY_HH
#define GENIE_SYSTEMCALLREGISTRY_HH

// Relix
#include "relix/syscall/registry.hh"


namespace Genie
{
	
	typedef relix::system_call SystemCall;
	
	const SystemCall* LookUpSystemCallByName( const char* name );
	
}

#endif
