/*	=======================
 *	Divergence/Utilities.hh
 *	=======================
 */

#ifndef DIVERGENCE_UTILITIES_HH
#define DIVERGENCE_UTILITIES_HH

#if !defined( __MACOS__ ) && !defined( __APPLE__ )

#error Divergence is Mac-only

#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif
#ifndef __AEINTERACTION__
#include <AEInteraction.h>
#endif

#if !TARGET_RT_MAC_MACHO
	#include "lamp/AESendBlocking.h"
#endif


namespace Divergence
{
	
	FSSpec ResolvePathToFSSpec( const char* path );
	
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
	
#else
	
	using ::AESendBlocking;
	
#endif
	
}

#endif

