// CarbonUnits/AEDataModel.hh

#ifndef CARBONUNITS_AEDATAMODEL_HH
#define CARBONUNITS_AEDATAMODEL_HH

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

// We need these definitions under the following conditions:
// 1. Accessors are functions on 68K (no CarbonAccessors.o), for compiling
// 2. Accessors are not functions, for including

// CARBONUNITS_LINKAGE is only defined (to 'pascal') if we're compiling.

#if defined(CARBONUNITS_LINKAGE) && ACCESSOR_CALLS_ARE_FUNCTIONS && TARGET_CPU_68K || !defined(CARBONUNITS_LINKAGE) && !ACCESSOR_CALLS_ARE_FUNCTIONS

#ifndef CARBONUNITS_LINKAGE
#define CARBONUNITS_LINKAGE inline
#endif


CARBONUNITS_LINKAGE OSErr AEGetDescData( const AEDesc*  desc,
                                         void*          dataPtr,
                                         Size           maximumSize )
{
	if ( desc->dataHandle == NULL )
	{
		return paramErr;
	}
	
	if ( *desc->dataHandle == NULL )
	{
		return nilHandleErr;
	}
	
	Size handleSize = GetHandleSize( desc->dataHandle );
	
	if ( handleSize < maximumSize )
	{
		maximumSize = handleSize;
	}
	
	::BlockMoveData( *desc->dataHandle, dataPtr, maximumSize );
	
	return noErr;
}

CARBONUNITS_LINKAGE Size AEGetDescDataSize( const AEDesc* desc )
{
	return GetHandleSize( desc->dataHandle );
}

CARBONUNITS_LINKAGE OSErr AEReplaceDescData( DescType     typeCode,
                                             const void*  dataPtr,
                                             Size         dataSize,
                                             AEDesc*      desc )
{
	bool typeIsNull = typeCode == typeNull;
	bool ptrIsNull  = dataPtr  == NULL;
	
	// The parameters must be consistently null or non-null.
	if ( typeIsNull != ptrIsNull )
	{
		return paramErr;
	}
	
	bool descWasNull = desc->dataHandle == NULL;
	
	if ( !descWasNull && !ptrIsNull )
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
	else if ( descWasNull && !ptrIsNull )
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


#undef CARBONUNITS_LINKAGE

#endif

#endif

