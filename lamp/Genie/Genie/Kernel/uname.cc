/*	========
 *	uname.cc
 *	========
 */

// Standard C++
#include <algorithm>

// POSIX
#include "sys/utsname.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	#if TARGET_CPU_68K
		
		#define HARDWARE_CLASS "m68k"
		
	#elif TARGET_CPU_PPC
		
		#define HARDWARE_CLASS "powerpc"
		
	#endif
	
	// Genie relies on CFM, so don't compile for Intel (below)
	
	
	static void CopyString( const char* str, char* dest )
	{
		std::size_t len = std::strlen( str );
		
		ASSERT( len < _UTSNAME_LENGTH );
		
		std::copy( str, str + len + 1, dest );  // copy length byte too
	}
	
	static int uname( struct utsname *uts )
	{
		CopyString( "Genie", uts->sysname );
		CopyString( "nodename", uts->nodename );
		CopyString( "0.7something", uts->release );
		CopyString( "verbose version string", uts->version );
		CopyString( HARDWARE_CLASS, uts->machine );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( uname );
	
}

