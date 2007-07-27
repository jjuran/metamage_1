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
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_uname )
	DEFINE_MODULE_INIT(  Kernel_uname )
	
	#if TARGET_CPU_68K
		
		#define HARDWARE_CLASS "m68k"
		
	#elif TARGET_CPU_PPC
		
		#define HARDWARE_CLASS "powerpc"
		
	#endif
	
	// Genie relies on CFM or classic 68K, so don't compile for Intel (below)
	
	
#if defined(_UTSNAME_LENGTH)
	
	static const unsigned kNameLength = _UTSNAME_LENGTH;
	
#elif defined(_SYS_NAMELEN)
	
	static const unsigned kNameLength = _SYS_NAMELEN;
	
#endif
	
	static void CopyString( const char* str, char* dest )
	{
		std::size_t len = std::strlen( str );
		
		ASSERT( len < kNameLength );
		
		std::copy( str, str + len + 1, dest );  // copy length byte too
	}
	
	static int uname( struct utsname *uts )
	{
		SystemCallFrame frame( "uname" );
		
		CopyString( "Genie", uts->sysname );
		CopyString( "nodename", uts->nodename );
		CopyString( "0.7something", uts->release );
		CopyString( "verbose version string", uts->version );
		CopyString( HARDWARE_CLASS, uts->machine );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( uname );
	
}

