/*	==============
 *	AEFramework.cp
 *	==============
 */

#include "AEFramework/AEFramework.h"


namespace Nitrogen
{
	
	static ExpectedReplies gExpectedReplies;
	
	void ExpectReply( AEReturnID   returnID,
	                  AppleEvent*  replyStorage,
	                  ThreadID     thread )
	{
		// assert( returnID != 0 );
		// Can replyStorage be NULL?  If you wanted to know when the reply came back
		// but didn't care what was in it, then it would make sense.
		
		gExpectedReplies[ returnID ] = ExpectedReply( thread, replyStorage );
	}
	
	void CancelReply( AEReturnID returnID )
	{
		gExpectedReplies.erase( returnID );
	}
	
	void CancelRepliesForThread( ThreadID thread )
	{
		typedef ExpectedReplies::iterator iterator;
		
		for ( iterator it = gExpectedReplies.begin();  it != gExpectedReplies.end();  )
		{
			if ( it->second.thread == thread )
			{
				gExpectedReplies.erase( it++ );
			}
			else
			{
				++it;
			}
		}
	}
	
	void ReceiveReply( const AppleEvent& reply )
	{
		AEReturnID returnID = AEGetAttributePtr< keyReturnIDAttr >( reply );
		
		ExpectedReplies::iterator found = gExpectedReplies.find( returnID );
		
		if ( found != gExpectedReplies.end() )
		{
			try
			{
				ThreadID    thread       = found->second.thread;
				AppleEvent* replyStorage = found->second.reply;
				
				// Make sure the thread exists
				ThreadState threadState = GetThreadState( thread );
				
				if ( threadState == kStoppedThreadState )
				{
					SetThreadState( thread, kReadyThreadState );
				}
				
				// before writing to its storage
				if ( replyStorage != NULL )
				{
					*replyStorage = AEDuplicateDesc( reply ).Release();
				}
			}
			catch ( ThreadNotFoundErr )
			{
				// A thread terminated without canceling a pending reply
			}
			
			gExpectedReplies.erase( found );
		}
		else
		{
			// No such return ID.  Possibly the expecting thread canceled or was terminated.
		}
	}
	
}

