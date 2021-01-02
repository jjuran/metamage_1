/*
	AESendBlocking.hh
	-----------------
*/

#ifndef AESENDBLOCKING_HH
#define AESENDBLOCKING_HH

#if ! defined( __MACOS__ )  &&  ! defined( __APPLE__ )
	#error AESendBlocking is Mac-only
#endif

#ifdef __RELIX__
	#include "relix/AESendBlocking.h"
#endif

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>

inline
OSErr AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply )
{
	return ::AESend( appleEvent,
	                 reply,
	                 kAEWaitReply | kAECanInteract,
	                 kAENormalPriority,
	                 kNoTimeOut,
	                 NULL,
	                 NULL );
}

#endif  // #ifdef __APPLE__

#endif
