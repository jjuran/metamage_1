/*
	fndr-sync.cc
	------------
*/

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

// POSIX
#include <unistd.h>

// mac-config
#include "mac_config/apple-events.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"


#pragma exceptions off


#define PREFIX  "fndr-sync: "

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( e, msg )  (wrote( 2, STR_LEN( PREFIX msg "\n" ) ) ? e : 13)

#define POD( obj )  (&(obj)), (sizeof (obj))


static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

#ifndef __FINDERREGISTRY__
// We don't need all of FinderRegistry.h (which includes AERegistry.h + OSA.h).
enum
{
	kAEFinderSuite = 'fndr',
	kAESync        = 'fupd',
};
#endif

#ifndef __GESTALT__
// We don't need all of Gestalt.h
enum
{
	gestaltAppleEventsAttr = 'evnt',
};
#endif

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||
			mac::sys::gestalt( gestaltAppleEventsAttr ) != 0);

static inline
AEAddressDesc create_Finder_address_AEDesc()
{
	const OSType sig = 'MACS';
	
	AEAddressDesc addr = {};
	
	OSErr err = AECreateDesc( typeApplSignature, &sig, sizeof sig, &addr );
	
	return addr;
}

static inline
OSStatus FSRef_from_path( const char* path, FSRef& result )
{
	return FSPathMakeRef( (const Byte*) path, &result, NULL );
}

int main( int argc, char** argv )
{
	if ( ! apple_events_present )
	{
		return ERROR( 78, "Apple events unavailable" );  // ENOSYS = 78
	}
	
	if ( argc != 2 )
	{
		return ERROR( 50, "one path argument required" );
	}
	
	const char* target_path = argv[ 1 ];
	
	using mac::relix::FSSpec_from_existing_path;
	
	AEAddressDesc address = create_Finder_address_AEDesc();
	
	OSErr err;
	AppleEvent event, reply;
	
	err = AECreateAppleEvent( kAEFinderSuite,  // 'fndr'
	                          kAESync,         // 'fupd'
	                          &address,
	                          kAutoGenerateReturnID,
	                          kAnyTransactionID,
	                          &event );
	
	if ( err )
	{
		return ERROR( 1, "AECreateAppleEvent failed" );
	}
	
	++argv;
	
#ifdef __APPLE__
	
	FSRef ref;
	
	if ( FSRef_from_path( target_path, ref ) )
	{
		return ERROR( 43, "path lookup failed" );
	}
	
	err = AEPutParamPtr( &event, keyDirectObject, typeFSRef, POD( ref ) );
	
#else
	
	FSSpec spec;
	
	if ( FSSpec_from_existing_path( target_path, spec ) )
	{
		return ERROR( 43, "path lookup failed" );
	}
	
	err = AEPutParamPtr( &event, keyDirectObject, typeFSS, POD( spec ) );
	
#endif
	
	if ( err )
	{
		ERROR( 0, "adding file parameter failed" );
	}
	
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
	
	if ( err )
	{
		return ERROR( 1, "AESend failed" );
	}
	
	return 0;
}
