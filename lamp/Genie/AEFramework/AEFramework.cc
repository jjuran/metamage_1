/*	==============
 *	AEFramework.cc
 *	==============
 */

#include "AEFramework/AEFramework.h"

// Standard C++
#include <map>

// mac-config
#include "mac_config/apple-events.hh"

// cthread-either
#include "cthread-either.hh"

#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif


#if CONFIG_APPLE_EVENTS
#define STATIC  static
#else
#define STATIC  extern
#endif


namespace Nitrogen
{
	
	using cthread::thread_id;
	
	using namespace cthread::either;
	
	
	struct ExpectedReply
	{
		thread_id         thread;
		Mac::AppleEvent*  reply;
		
		ExpectedReply()
		{
		}
		
		ExpectedReply( thread_id thread, Mac::AppleEvent* reply )
		:
			thread( thread ),
			reply ( reply  )
		{
		}
	};
	
	typedef std::map< SInt32, ExpectedReply > ExpectedReplies;
	
	STATIC ExpectedReplies gExpectedReplies;  // Not for System-6-only builds
	
	void ExpectReply( Mac::AEReturnID_32Bit   returnID,
	                  Mac::AppleEvent        *replyStorage )
	{
		// assert( returnID != 0 );
		// Can replyStorage be NULL?  If you wanted to know when the reply came back
		// but didn't care what was in it, then it would make sense.
		
		gExpectedReplies[ returnID ] = ExpectedReply( current_thread(), replyStorage );
	}
	
	static inline
	SInt32 AEGetAttributePtr_keyReturnIDAttr( const AppleEvent& appleEvent )
	{
		const Mac::AEKeyword key = Mac::keyReturnIDAttr;
		const Mac::DescType type = Mac::typeSInt32;
		
		SInt32 result;
		
		return AEKeyword_get< key >( AEGetAttributePtr_Getter< type >( appleEvent, key ) );
	}
	
	void ReceiveReply( const Mac::AppleEvent& reply )
	{
		SInt32 returnID = AEGetAttributePtr_keyReturnIDAttr( reply );
		
		ExpectedReplies::iterator found = gExpectedReplies.find( returnID );
		
		if ( found != gExpectedReplies.end() )
		{
			thread_id        thread       = found->second.thread;
			Mac::AppleEvent* replyStorage = found->second.reply;
			
			gExpectedReplies.erase( found );
			
			if ( woken_thread( thread ) )
			{
				// Make sure the thread exists before writing to its storage
				if ( replyStorage != NULL )
				{
					*replyStorage = AEDuplicateDesc( reply ).release();
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
	}
	
}
