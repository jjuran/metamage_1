/*
	event_handlers.hh
	-----------------
*/

#include "mac_app/event_handlers.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#ifndef MAC_OS_X_VERSION_10_5
// CoreServices in 10.4 lacks AppleEvents.h.
#include <ApplicationServices/ApplicationServices.h>
#endif
#endif

// Mac OS
#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

// Annex
#ifndef ANNEX_MACTYPES_H
#include "Annex/MacTypes.h"
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-app-utils
#include "mac_app/commands.hh"


namespace mac {
namespace app {

static
pascal OSErr Quit( const AppleEvent* event, AppleEvent* reply, SRefCon )
{
	return quit();
}

void install_basic_event_handlers()
{
	DEFINE_UPP( AEEventHandler, Quit );
	
	OSErr err;
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEQuitApplication,
	                             UPP_ARG( Quit ),
	                             0,
	                             false );
}

template < class T >
static inline
OSErr AEGetNthPtr( const AEDesc* list, long i, DescType desired, T* it )
{
	AEKeyword key;
	DescType type;
	Size size;
	
	return AEGetNthPtr( list, i, desired, &key, &type, it, sizeof (T), &size );
}

template < class File >
struct File_DescType_traits;

#if ! __LP64__
template<> struct File_DescType_traits< FSSpec > { enum { value = typeFSS }; };
#endif
template<> struct File_DescType_traits< FSRef > { enum { value = typeFSRef }; };

template < class File, class Callback >
static
OSErr ODoc_generic( const AppleEvent* event, Callback callback )
{
	if ( ! callback )
	{
		return errAEEventNotHandled;
	}
	
	const DescType desiredType = File_DescType_traits< File >::value;
	
	OSErr err;
	
	AEDescList list;
	
	err = AEGetParamDesc( event, keyDirectObject, typeAEList, &list );
	
	if ( err == noErr )
	{
		Size count;
		err = AECountItems( &list, &count );
		
		if ( err == noErr )
		{
			for ( int i = 1;  i <= count;  ++i )
			{
				File file;
				
				err = AEGetNthPtr( &list, i, desiredType, &file );
				
				if ( err == noErr )
				{
					OSStatus stat = callback( file );
					
					if ( stat < 0 )
					{
						err = stat;
						break;
					}
				}
			}
		}
		
		AEDisposeDesc( &list );
	}
	
	return err;
}

#if ! __LP64__

struct HFS_callback_adapter
{
	HFS_callback callback;
	
	operator HFS_callback() const  { return callback; }
	
	long operator()( const FSSpec& spec )
	{
		return callback( spec.vRefNum, spec.parID, spec.name );
	}
};

static
pascal OSErr ODoc_HFS( const AppleEvent* event, AppleEvent*, SRefCon refcon )
{
	return ODoc_generic< FSSpec >( event, (HFS_callback_adapter&) refcon );
}

void install_opendocs_handler( HFS_callback callback )
{
	DEFINE_UPP( AEEventHandler, ODoc_HFS );
	
	OSErr err;
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEOpenDocuments,
	                             UPP_ARG( ODoc_HFS ),
	                             (SRefCon) callback,
	                             false );
}

static
pascal OSErr ODoc_FSSpec( const AppleEvent* event, AppleEvent*, SRefCon refcon )
{
	return ODoc_generic< FSSpec >( event, (FSSpec_callback) refcon );
}

void install_opendocs_handler( FSSpec_callback callback )
{
	DEFINE_UPP( AEEventHandler, ODoc_FSSpec );
	
	OSErr err;
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEOpenDocuments,
	                             UPP_ARG( ODoc_FSSpec ),
	                             (SRefCon) callback,
	                             false );
}

#endif

static
pascal OSErr ODoc_FSRef( const AppleEvent* event, AppleEvent*, SRefCon refcon )
{
	return ODoc_generic< FSRef >( event, (FSRef_callback) refcon );
}

void install_opendocs_handler( FSRef_callback callback )
{
	DEFINE_UPP( AEEventHandler, ODoc_FSRef );
	
	OSErr err;
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEOpenDocuments,
	                             UPP_ARG( ODoc_FSRef ),
	                             (SRefCon) callback,
	                             false );
}

}
}
