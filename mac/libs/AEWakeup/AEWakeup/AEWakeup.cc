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

// Standard C++
#include <map>

// mac-config
#include "mac_config/apple-events.hh"

// cthread-either
#include "cthread-either.hh"


#if CONFIG_APPLE_EVENTS
#define STATIC  static
#else
#define STATIC  extern
#endif


namespace AEWakeup
{

using cthread::thread_id;

using namespace cthread::either;


struct ExpectedReply
{
	thread_id    thread;
	AppleEvent*  reply;
	
	ExpectedReply()
	{
	}
	
	ExpectedReply( thread_id thread, AppleEvent* reply )
	:
		thread( thread ),
		reply ( reply  )
	{
	}
};

typedef std::map< long, ExpectedReply > ExpectedReplies;

STATIC ExpectedReplies gExpectedReplies;  // Not for System-6-only builds

void Request( long         returnID,
              AppleEvent*  replyStorage )
{
	// assert( returnID != 0 );
	// Can replyStorage be NULL?  If you wanted to know when the reply came back
	// but didn't care what was in it, then it would make sense.
	
	gExpectedReplies[ returnID ] = ExpectedReply( current_thread(), replyStorage );
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
	
	ExpectedReplies::iterator found = gExpectedReplies.find( returnID );
	
	if ( found != gExpectedReplies.end() )
	{
		thread_id   thread       = found->second.thread;
		AppleEvent* replyStorage = found->second.reply;
		
		gExpectedReplies.erase( found );
		
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
