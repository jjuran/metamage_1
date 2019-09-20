/*	=============
 *	AEFramework.h
 *	=============
 */

#ifndef AEFRAMEWORK_AEFRAMEWORK_H
#define AEFRAMEWORK_AEFRAMEWORK_H

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif


namespace Genie
{
	
	void ExpectReply( long         returnID,
	                  AppleEvent*  replyStorage );
	
	short ReceiveReply( const AppleEvent& reply );
	
}

#endif
