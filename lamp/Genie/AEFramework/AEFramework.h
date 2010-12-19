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


namespace Nitrogen
{
	
	void ExpectReply( AEReturnID_32Bit   returnID,
	                  AppleEvent        *replyStorage );
	
	void ReceiveReply( const AppleEvent& reply );
	
}

#endif

