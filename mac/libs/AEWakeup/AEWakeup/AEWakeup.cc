/*
	AEWakeup.cc
	-----------
*/

#include "AEWakeup/AEWakeup.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#ifndef MAC_OS_X_VERSION_10_5
#include <ApplicationServices/ApplicationServices.h>
#endif
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// Standard C
#include <stdlib.h>

// mac-config
#include "mac_config/apple-events.hh"

// cthread-either
#include "cthread-either.hh"


#pragma exceptions off


#if CONFIG_APPLE_EVENTS
#define STATIC  static
#else
#define STATIC  extern
#endif


namespace AEWakeup
{

using cthread::thread_id;

using namespace cthread::either;


struct wakeup_request
{
	wakeup_request* next;
	
	thread_id    thread;
	AppleEvent*  reply;
	SInt32       return_id;
};

STATIC wakeup_request* request_list;  // Not for System-6-only builds
STATIC wakeup_request* request_free;

wakeup_request* Preflight()
{
	if ( request_free )
	{
		return request_free;
	}
	
	wakeup_request* spare = (wakeup_request*) malloc( sizeof (wakeup_request) );
	
	if ( spare != NULL )
	{
		request_free = spare;
	}
	
	return spare;
}

void Request( long         returnID,
              AppleEvent*  replyStorage )
{
	// assert( returnID != 0 );
	// Can replyStorage be NULL?  If you wanted to know when the reply came back
	// but didn't care what was in it, then it would make sense.
	
	if ( wakeup_request* request = Preflight() )
	{
		request->next      = request_list;
		request->reply     = replyStorage;
		request->thread    = current_thread();
		request->return_id = returnID;
		
		request_list = request;
		request_free = NULL;
	}
	
	/*
		If preflight fails and you already sent the event, you won't
		get a wakeup for the reply.  Call Preflight() first!
	*/
}

static
wakeup_request** find_link( SInt32 key )
{
	wakeup_request** link = &request_list;
	
	while ( wakeup_request* node = *link )
	{
		if ( node->return_id == key )
		{
			return link;
		}
		
		link = &node->next;
	}
	
	return NULL;
}

OSErr Deliver( const AppleEvent& reply )
{
	OSErr err;
	SInt32 data;
	
	AEKeyword key = keyReturnIDAttr;
	DescType type = typeSInt32;
	Size     size = sizeof data;
	
	err = AEGetAttributePtr( &reply, key, type, &type, &data, size, &size );
	
	if ( err != noErr )
	{
		return err;
	}
	
	SInt32 returnID = data;
	
	if ( wakeup_request** link = find_link( returnID ) )
	{
		wakeup_request* request = *link;
		
		*link = request->next;  // Remove the request from the list
		
		thread_id   thread       = request->thread;
		AppleEvent* replyStorage = request->reply;
		
		if ( request_free )
		{
			free( request );
		}
		else
		{
			request_free = request;
		}
		
		if ( woken_thread( thread ) )
		{
			// Make sure the thread exists before writing to its storage
			if ( replyStorage != NULL )
			{
				err = AEDuplicateDesc( &reply, replyStorage );
				
				if ( err )
				{
					replyStorage->descriptorType = 0;
					replyStorage->dataHandle     = 0;
					
					return err;
				}
			}
			
			yield_to_thread( thread );
		}
		else
		{
			// A thread terminated without canceling a pending reply
		}
	}
	else
	{
		// No such return ID.  Possibly the expecting thread canceled or was terminated.
	}
	
	return noErr;
}

}
