// Carbonate/AEDataModel.hh

#ifndef CARBONUNITS_AEDATAMODEL_HH
#define CARBONUNITS_AEDATAMODEL_HH

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

// We need these definitions under the following conditions:
// 1. Accessors are functions on 68K (no CarbonAccessors.o), for compiling
// 2. Accessors are not functions, for including

// CARBONATE_LINKAGE is only defined (to 'pascal') if we're compiling.

#if defined(CARBONATE_LINKAGE) && ACCESSOR_CALLS_ARE_FUNCTIONS && TARGET_CPU_68K || !defined(CARBONATE_LINKAGE) && !ACCESSOR_CALLS_ARE_FUNCTIONS

#ifndef CARBONATE_LINKAGE
#define CARBONATE_LINKAGE inline pascal
#endif


CARBONATE_LINKAGE OSErr AEGetDescData( const AEDesc*  desc,
                                       void*          dataPtr,
                                       Size           maximumSize )
{
	if ( desc->dataHandle == NULL )
	{
		return noErr;
	}
	
	if ( *desc->dataHandle == NULL )
	{
		return noErr;
	}
	
	Size handleSize = GetHandleSize( desc->dataHandle );
	
	if ( handleSize < maximumSize )
	{
		maximumSize = handleSize;
	}
	
	::BlockMoveData( *desc->dataHandle, dataPtr, maximumSize );
	
	return noErr;
}

CARBONATE_LINKAGE Size AEGetDescDataSize( const AEDesc* desc )
{
	if ( desc->descriptorType == typeNull )
	{
		return 0;
	}
	
	return GetHandleSize( desc->dataHandle );
}

CARBONATE_LINKAGE OSErr AEReplaceDescData( DescType     typeCode,
                                           const void*  dataPtr,
                                           Size         dataSize,
                                           AEDesc*      desc )
{
	bool typeIsNull = typeCode == typeNull;
	
	bool descWasNull = desc->dataHandle == NULL;
	
	if ( !descWasNull && !typeIsNull )
	{
		// Replace the data.  Resize the handle, copy the data, and set the type.
		SetHandleSize( desc->dataHandle, dataSize );
		
		if ( MemError() != noErr )
		{
			return MemError();
		}
		
		::BlockMoveData( dataPtr, *desc->dataHandle, dataSize );
		
		desc->descriptorType = typeCode;
	}
	else if ( descWasNull && !typeIsNull )
	{
		// Create a new descriptor record.
		return AECreateDesc( typeCode, dataPtr, dataSize, desc );
	}
	else if ( !descWasNull )
	{
		// Delete the existing descriptor record.
		return ::AEDisposeDesc( desc );
	}
	else
	{
		// Replace null with null == do nothing.
	}
	
	return noErr;
}


#undef CARBONATE_LINKAGE

#endif

#endif

