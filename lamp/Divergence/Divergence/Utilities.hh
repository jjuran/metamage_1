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

#if TARGET_RT_MAC_CFM
#include "SystemCalls.hh"
#endif


namespace Divergence
{
	
	inline FSSpec ResolvePathToFSSpec( const char* path )
	{
	#if TARGET_RT_MAC_MACHO
		
		return Nucleus::Convert< FSSpec >( Nitrogen::FSPathMakeRef( path ).ref );
		
	#elif TARGET_RT_MAC_CFM
		
		return Path2FSS( path );
		
	#else
		
		#error Unsupported runtime (not CFM or Mach-O)
		
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
	
#elif TARGET_RT_MAC_CFM
	
	using ::AESendBlocking;
	
#endif
	
}

#endif

