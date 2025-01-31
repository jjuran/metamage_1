/*
	factory.c.h
	-----------
*/

// Mac OS X
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

// qlgenerator
#include "Preview.h"
#include "Thumbnail.h"


// Don't modify this line
#define PLUGIN_ID "A6BD9F85-362C-A833-B179-DF559F700C02"


struct QLGenerator
{
	void*      conduitInterface;
	CFUUIDRef  factoryID;
	UInt32     refCount;
};

typedef struct QLGenerator QLGenerator;

typedef QLGeneratorInterfaceStruct QLGeneratorConduit;


static
Boolean UUIDBytes_equal_to_UUID( CFUUIDBytes bytes, CFUUIDRef b )
{
	CFUUIDRef a = CFUUIDCreateFromUUIDBytes( kCFAllocatorDefault, bytes );
	
	Boolean equal = CFEqual( a, b );
	
	CFRelease( a );
	
	return equal;
}

static
void deallocate( QLGenerator* self )
{
	CFUUIDRef factoryID = self->factoryID;
	
	if ( factoryID )
	{
		CFPlugInRemoveInstanceForFactory( factoryID );
		CFRelease                       ( factoryID );
	}
	
	free( self->conduitInterface );

	free( self );
}

static
UInt32 Release( void* state )
{
	QLGenerator* self = (QLGenerator*) state;
	
	UInt32 count = --self->refCount;
	
	if ( count == 0 )
	{
		deallocate( self );
	}
	
	return count;
}

static
UInt32 AddRef( void* state )
{
	QLGenerator* self = (QLGenerator*) state;
	
	return ++self->refCount;
}

static
SInt32 QueryInterface( void* state, CFUUIDBytes iid, void** ppv )
{
	QLGenerator* self = (QLGenerator*) state;
	
	QLGeneratorConduit* conduit = (QLGeneratorConduit*) self->conduitInterface;
	
	*ppv = NULL;
	
	if ( UUIDBytes_equal_to_UUID( iid, kQLGeneratorCallbacksInterfaceID ))
	{
		*ppv = state;
		
		conduit->AddRef( state );
		
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

static const QLGeneratorConduit conduit_template =
{
	NULL,
	QueryInterface,
	AddRef,
	Release,
	GenerateThumbnailForURL,
	CancelThumbnailGeneration,
	GeneratePreviewForURL,
	CancelPreviewGeneration,
};

static
CFUUIDRef make_uuid()
{
	return CFUUIDCreateFromString( kCFAllocatorDefault, CFSTR( PLUGIN_ID ) );
}

static
QLGenerator* allocate()
{
	CFUUIDRef factoryID = make_uuid();
	
	QLGenerator* plugin = (QLGenerator*) malloc( sizeof (QLGenerator) );
	
	void* conduit = malloc( sizeof (QLGeneratorConduit) );
	
	memcpy( conduit, &conduit_template, sizeof conduit_template );
	
	plugin->conduitInterface = conduit;
	
	plugin->factoryID = factoryID;
	plugin->refCount = 1;
	
	CFPlugInAddInstanceForFactory( factoryID );
	
	return plugin;
}

__attribute__ ((visibility ("default")))
void* QLGeneratorFactory( CFAllocatorRef allocator, CFUUIDRef typeID )
{
	if ( CFEqual( typeID, kQLGeneratorTypeID ) )
	{
		return allocate();
	}
	
	return NULL;
}
