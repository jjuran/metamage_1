/*
	AEWakeup.hh
	-----------
*/

#ifndef AEWAKEUP_AEWAKEUP_HH
#define AEWAKEUP_AEWAKEUP_HH

// #include <AEDataModel.h>
struct AEDesc;
typedef AEDesc AppleEvent;


namespace AEWakeup
{
	
	struct wakeup_request;
	
	wakeup_request* Preflight();
	
	void Request( long         returnID,
	              AppleEvent*  replyStorage );
	
	short Deliver( const AppleEvent& reply );
	
}

#endif
