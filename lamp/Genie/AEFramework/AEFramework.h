/*	=============
 *	AEFramework.h
 *	=============
 */

#ifndef AEFRAMEWORK_AEFRAMEWORK_H
#define AEFRAMEWORK_AEFRAMEWORK_H

// Nitrogen
#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif
#ifndef NITROGEN_THREADS_HH
#include "Nitrogen/Threads.hh"
#endif


namespace Nitrogen
{
	
	void ExpectReply( AEReturnID_32Bit   returnID,
	                  AppleEvent        *replyStorage,
	                  ThreadID           thread = GetCurrentThread() );
	
	void CancelReply( AEReturnID_32Bit returnID );
	
	void CancelRepliesForThread( ThreadID thread );
	
	void ReceiveReply( const AppleEvent& reply );
	
}

#endif

