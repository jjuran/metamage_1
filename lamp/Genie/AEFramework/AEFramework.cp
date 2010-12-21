/*	==============
 *	AEFramework.cp
 *	==============
 */

#include "AEFramework/AEFramework.h"

// Standard C++
#include <map>

// Nitrogen
#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif
#ifndef NITROGEN_THREADS_HH
#include "Nitrogen/Threads.hh"
#endif


namespace Nitrogen
{
	
	struct ExpectedReply
	{
		Mac::ThreadID     thread;
		Mac::AppleEvent*  reply;
		
		ExpectedReply()
		{
		}
		
		ExpectedReply( Mac::ThreadID thread, Mac::AppleEvent* reply )
		:
			thread( thread ),
			reply ( reply  )
		{
		}
	};
	
	typedef std::map< AEReturnID_32Bit, ExpectedReply > ExpectedReplies;
	
	static ExpectedReplies gExpectedReplies;
	
	void ExpectReply( Mac::AEReturnID_32Bit   returnID,
	                  Mac::AppleEvent        *replyStorage )
	{
		// assert( returnID != 0 );
		// Can replyStorage be NULL?  If you wanted to know when the reply came back
		// but didn't care what was in it, then it would make sense.
		
		gExpectedReplies[ returnID ] = ExpectedReply( GetCurrentThread(), replyStorage );
	}
	
	void ReceiveReply( const Mac::AppleEvent& reply )
	{
		AEReturnID_32Bit returnID = AEGetAttributePtr< Mac::keyReturnIDAttr >( reply );
		
		ExpectedReplies::iterator found = gExpectedReplies.find( returnID );
		
		if ( found != gExpectedReplies.end() )
		{
			try
			{
				Mac::ThreadID    thread       = found->second.thread;
				Mac::AppleEvent* replyStorage = found->second.reply;
				
				// Make sure the thread exists
				ThreadState threadState = GetThreadState( thread );
				
				if ( threadState == kStoppedThreadState )
				{
					SetThreadState( thread, kReadyThreadState );
				}
				
				// before writing to its storage
				if ( replyStorage != NULL )
				{
					*replyStorage = AEDuplicateDesc( reply ).release();
				}
			}
			catch ( const OSStatus& err )
			{
				if ( err != threadNotFoundErr )
				{
					throw;
				}
				
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

