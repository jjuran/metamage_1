/*	=============
 *	AEFramework.h
 *	=============
 */

#ifndef AEFRAMEWORK_AEFRAMEWORK_H
#define AEFRAMEWORK_AEFRAMEWORK_H

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AEDESC_HH
#include "Mac/AppleEvents/Types/AEDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AERETURNID_HH
#include "Mac/AppleEvents/Types/AEReturnID.hh"
#endif


namespace Nitrogen
{
	
	void ExpectReply( Mac::AEReturnID_32Bit  returnID,
	                  Mac::AppleEvent*       replyStorage );
	
	void ReceiveReply( const Mac::AppleEvent& reply );
	
}

#endif

