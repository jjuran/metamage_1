/*	=======================
 *	Divergence/Utilities.hh
 *	=======================
 */

#ifndef DIVERGENCE_UTILITIES_HH
#define DIVERGENCE_UTILITIES_HH

// Universal Interfaces
#include <AEDataModel.h>

#if TARGET_RT_MAC_MACHO
#include "Nucleus/Convert.h"
#include "Nitrogen/Files.h"
#endif

#if TARGET_OS_MAC && !TARGET_RT_MAC_MACHO
#include "SystemCalls.hh"
#endif


namespace Divergence
{
	
	inline FSSpec ResolvePathToFSSpec( const char* path )
	{
	#if TARGET_RT_MAC_MACHO
		
		return Nucleus::Convert< FSSpec >( Nitrogen::FSPathMakeRef( path ).ref );
		
	#elif TARGET_OS_MAC
		
		return Path2FSS( path );
		
	#else
		
		#error Unsupported platform (not Lamp or Mach-O)
		
	#endif
	}
	
#if TARGET_RT_MAC_MACHO
	
	inline OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply )
	{
		return ::AESend( appleEvent,
		                 reply,
		                 kAEWaitReply | kAECanInteract,
		                 kAENormalPriority,
		                 kNoTimeOut,
		                 NULL,
		                 NULL );
	}
	
#elif TARGET_OS_MAC
	
	using ::AESendBlocking;
	
#endif
	
}

#endif

