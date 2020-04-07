/*	=============
 *	AEFramework.h
 *	=============
 */

#ifndef AEFRAMEWORK_AEFRAMEWORK_H
#define AEFRAMEWORK_AEFRAMEWORK_H

// #include <AEDataModel.h>
struct AEDesc;
typedef AEDesc AppleEvent;


namespace Genie
{
	
	void ExpectReply( long         returnID,
	                  AppleEvent*  replyStorage );
	
	short ReceiveReply( const AppleEvent& reply );
	
}

#endif
