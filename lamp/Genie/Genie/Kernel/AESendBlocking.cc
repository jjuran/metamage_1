/*
	AESendBlocking.cc
	-----------------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEInteraction.h>
#endif

// mac-config
#include "mac_config/apple-events.hh"

// Relix
#include "relix/AESendBlocking.h"
#include "relix/syscall/registry.hh"

// relix
#include "relix/api/current_thread.hh"
#include "relix/signal/check_signals.hh"
#include "relix/signal/signal.hh"
#include "relix/task/scheduler.hh"
#include "relix/task/thread.hh"

// AEWakeup
#include "AEWakeup/AEWakeup.hh"


OSStatus AESendBlocking( const AppleEvent* event, AppleEvent* reply )
{
	using namespace relix;
	
	if ( ! AEWakeup::Preflight() )
	{
		return memFullErr;
	}
	
	const AESendMode     sendMode = kAEQueueReply | kAECanInteract;
	const AESendPriority priority = kAENormalPriority;
	const SInt32         timeout  = kAEDefaultTimeout;
	
	OSErr err;
	
	reply->dataHandle = NULL;
	
	err = AESend( event, reply, sendMode, priority, timeout, NULL, NULL );
	
	if ( err != noErr )
	{
		return err;
	}
	
	SInt32 returnID;
	
	AEKeyword key = keyReturnIDAttr;
	DescType type = typeSInt32;
	Size     size = sizeof returnID;
	
	// Now that we've sent the event, retrieve the return ID
	err = AEGetAttributePtr( event, key, type, &type, &returnID, size, &size );
	
	if ( err != noErr )
	{
		return err;  // This really shouldn't happen
	}
	
	// Subscribe to AEWakeup's queued reply delivery and wake-up service
	AEWakeup::Request( returnID, reply );
	
	thread& current = current_thread();
	
	suspend_task( current.get_task() );
	
stop_and_wait:
	
	current.mark_current_stack_frame();
	
	// Sleep until the reply is delivered
	if ( OSErr err = stop_os_thread_nothrow( current.get_os_thread() ) )
	{
		return err;
	}
	
	/*
		Check for fatal signals; ignore caught ones (for now).
		
		TODO:  Return errAEWaitCanceled for a caught signal, and check
		on entry to determine if the Apple event has already been sent by
		a prior entry to this call.  We could possibly use keyReturnIDAttr
		for this, as long as the caller doesn't provide us with an event
		that's been sent previously.
	*/
	
	try
	{
		check_signals( false );  // Don't throw caught signals
	}
	catch ( const relix::signal& sig )
	{
		relix::the_signal = sig;
		
		// Only a fatal signal gets here, so actual error code doesn't matter
		return errAEWaitCanceled;
	}
	
	if ( reply->dataHandle == NULL )
	{
		goto stop_and_wait;
	}
	
	resume_task( current.get_task() );
	
	return noErr;
}

#if CONFIG_APPLE_EVENTS

#pragma force_active on

REGISTER_SYSTEM_CALL( AESendBlocking );

#pragma force_active reset

#endif
