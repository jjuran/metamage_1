/*
	apple_events.cc
	---------------
*/

#include "apple_events.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Standard C
#include <stdlib.h>

// Annex
#ifndef ANNEX_MACTYPES_H
#include "Annex/MacTypes.h"
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// plus
#include "plus/var_string.hh"


enum open_status
{
	Open_pending,
	Open_document,
	Open_default = -1,
};

static open_status opening;

static
OSStatus set_AMS_OPEN_for_FSRef( const FSRef& ref )
{
	OSStatus err;
	FSCatalogInfo info;
	
	err = FSGetCatalogInfo( &ref,
	                        kFSCatInfoFinderInfo,
	                        &info,
	                        NULL,    // name
	                        NULL,    // FSSpec
	                        NULL );  // parent
	
	if ( err )
	{
		return err;
	}
	
	FileInfo& fileInfo = (FileInfo&) info.finderInfo;
	
	if ( fileInfo.fileType == 0 )
	{
		return extFSErr;
	}
	
	const int prefix_len = 5;
	
	plus::var_string path;
	
	UInt32 maxPathSize = 64;
	
	do
	{
		maxPathSize *= 2;
		
		char* p = path.reset( maxPathSize ) + prefix_len;
		
		err = FSRefMakePath( &ref, (UInt8*) p, maxPathSize - prefix_len );
	}
	while ( err == pathTooLongErr  ||  err == buffersTooSmall );
	
	if ( err )  return err;
	
	path.resize( path.find( '\0', prefix_len ) );
	
	char* p = &path[ 0 ];
	
	UInt8* q = info.finderInfo;
	
	if ( TARGET_RT_BIG_ENDIAN )
	{
		*p++ = *q++;
		*p++ = *q++;
		*p++ = *q++;
		*p++ = *q++;
	}
	else
	{
		q += 4;
		
		*p++ = *--q;
		*p++ = *--q;
		*p++ = *--q;
		*p++ = *--q;
	}
	
	*p++ = ':';
	
	setenv( "AMS_OPEN", path.c_str(), 1 );
	
	return noErr;
}

static inline
OSErr get_AE_type_attr( const AppleEvent* event, AEKeyword key, OSType* q )
{
	return AEGetAttributePtr( event, key, typeType, NULL, q, sizeof *q, NULL );
}

static
pascal OSErr handle_required_Apple_event( AppleEvent const* event,
                                          AppleEvent*       reply,
                                          SRefCon           refcon )
{
	OSErr err;
	AEEventID evID;
	
	if (( err = get_AE_type_attr( event, keyEventIDAttr, &evID ) ))
	{
		return err;
	}
	
	switch ( evID )
	{
		case kAEOpenDocuments:
			FSRef opened_doc_file;
			
			(err = AEGetParamPtr( event,
			                     keyDirectObject,
			                     typeFSRef,
			                     NULL,
			                     &opened_doc_file,
			                     sizeof opened_doc_file,
			                     NULL ))
			||
			(err = set_AMS_OPEN_for_FSRef( opened_doc_file ));
			
			if ( err )  return err;
			
			opening = Open_document;
			break;
		
		case kAEOpenApplication:
			opening = Open_default;
			break;
		
		default:
			return errAEEventNotHandled;
	}
	
	return noErr;
}

DEFINE_UPP( AEEventHandler, handle_required_Apple_event )

int wait_for_first_Apple_event()
{
	OSStatus err;
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             typeWildCard,
	                             UPP_ARG( handle_required_Apple_event ),
	                             0,
	                             false );
	
	if ( err )  return err;
	
	EventTargetRef target = GetEventDispatcherTarget();
	
	while ( ! opening )
	{
		EventRef event;
		
		const bool pull = true;
		
		err = ReceiveNextEvent( 0, NULL, kEventDurationForever, pull, &event );
		
		if ( err )  return err;
		
		OSType eventClass = GetEventClass( event );
		
		if ( eventClass == kEventClassApplication )
		{
			if ( GetEventKind( event ) == kEventAppQuit )
			{
				return userCanceledErr;
			}
		}
		
	#ifdef MAC_OS_X_VERSION_10_5
		
		if ( eventClass == kEventClassAppleEvent )
		{
			err = AEProcessEvent( event );
			
			goto next;
		}
		
	#else  // #ifdef MAC_OS_X_VERSION_10_5
		
		EventRecord eventRec;
		
		if ( bool ok = ConvertEventRefToEventRecord( event, &eventRec ) )
		{
			if ( eventRec.what == kHighLevelEvent )
			{
				(void) AEProcessAppleEvent( &eventRec );
				goto next;
			}
		}
		
	#endif  // #ifdef MAC_OS_X_VERSION_10_5
		
		err = SendEventToEventTarget( event, target );
		
	next:
		
		if ( pull )
		{
			ReleaseEvent( event );
		}
	}
	
	err = AERemoveEventHandler( kCoreEventClass,
	                            typeWildCard,
	                            UPP_ARG( handle_required_Apple_event ),
	                            false );
	
	return noErr;
}
