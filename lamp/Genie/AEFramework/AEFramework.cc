/*	==============
 *	AEFramework.cc
 *	==============
 */

#include "AEFramework/AEFramework.h"

// Standard C++
#include <map>

// cthread-either
#include "cthread-either.hh"

// Nitrogen
#ifndef MAC_TOOLBOX_TYPES_OSSTATUS_HH
#include "Mac/Toolbox/Types/OSStatus.hh"
#endif

#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
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
	
	typedef std::map< AEReturnID_32Bit, ExpectedReply > ExpectedReplies;
	
	static ExpectedReplies gExpectedReplies;
	
	void ExpectReply( Mac::AEReturnID_32Bit   returnID,
	                  Mac::AppleEvent        *replyStorage )
	{
		// assert( returnID != 0 );
		// Can replyStorage be NULL?  If you wanted to know when the reply came back
		// but didn't care what was in it, then it would make sense.
		
		gExpectedReplies[ returnID ] = ExpectedReply( current_thread(), replyStorage );
	}
	
	void ReceiveReply( const Mac::AppleEvent& reply )
	{
		AEReturnID_32Bit returnID = AEGetAttributePtr< Mac::keyReturnIDAttr >( reply );
		
		ExpectedReplies::iterator found = gExpectedReplies.find( returnID );
		
		if ( found != gExpectedReplies.end() )
		{
			try
			{
				thread_id        thread       = found->second.thread;
				Mac::AppleEvent* replyStorage = found->second.reply;
				
				// Make sure the thread exists
				
				if ( is_thread_stopped( thread ) )
				{
					wake_thread( thread );
				}
				
				// before writing to its storage
				if ( replyStorage != NULL )
				{
					*replyStorage = AEDuplicateDesc( reply ).release();
				}
			}
			catch ( const Mac::OSStatus& err )
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
