/*	=============
 *	AEFramework.h
 *	=============
 */

#ifndef AEFRAMEWORK_AEFRAMEWORK_H
#define AEFRAMEWORK_AEFRAMEWORK_H

// Standard C++
#include <map>

// Nitrogen
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_THREADS_H
#include "Nitrogen/Threads.h"
#endif


namespace Nitrogen
{
	
	struct ExpectedReply
	{
		ThreadID thread;
		AppleEvent* reply;
		
		ExpectedReply()  {}
		
		ExpectedReply( ThreadID thread, AppleEvent* reply )
		:
			thread( thread ),
			reply ( reply  )
		{}
	};
	
	typedef std::map< AEReturnID, ExpectedReply > ExpectedReplies;
	
	void ExpectReply( AEReturnID   returnID,
	                  AppleEvent*  replyStorage,
	                  ThreadID     thread = GetCurrentThread() );
	
	void CancelReply( AEReturnID returnID );
	
	void CancelRepliesForThread( ThreadID thread );
	
	void ReceiveReply( const AppleEvent& reply );
	
}

#endif
