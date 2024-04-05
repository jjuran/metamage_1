/*
	FinderSync.cc
	-------------
*/

#include "Genie/Utilities/FinderSync.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif
#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

// mac-config
#include "mac_config/apple-events.hh"

// Pedestal
#include "Pedestal/Application.hh"

#ifndef __FINDERREGISTRY__
// We don't need all of FinderRegistry.h (which includes AERegistry.h + OSA.h).
enum
{
	kAEFinderSuite = 'fndr',
	kAESync        = 'fupd',
};
#endif


#define POD( obj )  (&(obj)), (sizeof (obj))


namespace Genie
{

static
AEAddressDesc create_Finder_address_AEDesc()
{
	const OSType sig = 'MACS';
	
	AEAddressDesc addr = {};
	
	OSErr err = AECreateDesc( typeApplSignature, &sig, sizeof sig, &addr );
	
	return addr;
}

void send_Finder_sync_event( const FSSpec& spec )
{
	const bool apple_events_present =
		CONFIG_APPLE_EVENTS  &&
			(CONFIG_APPLE_EVENTS_GRANTED  ||
				Pedestal::apple_events_present);
	
	if ( ! apple_events_present )
	{
		return;
	}
	
	static AEAddressDesc address = create_Finder_address_AEDesc();
	
	OSErr err;
	AppleEvent event, reply;
	
	err = AECreateAppleEvent( kAEFinderSuite,  // 'fndr'
	                          kAESync,         // 'fupd'
	                          &address,
	                          kAutoGenerateReturnID,
	                          kAnyTransactionID,
	                          &event );
	
	if ( err == noErr )
	{
		err = AEPutParamPtr( &event, keyDirectObject, typeFSS, POD( spec ) );
		
		/*
			Continue with AESend even if adding the parameter failed.
			At least in Mac OS 9, the Finder will perform a sync anyway.
		*/
		
		err = AESend( &event,
		              &reply,
		              kAENoReply,
		              kAENormalPriority,
		              kNoTimeOut,
		              NULL,
		              NULL );
		
		AEDisposeDesc( &event );
	}
}	

}
