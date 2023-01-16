// Carbonate/AEDataModel.cc

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

#if !ACCESSOR_CALLS_ARE_FUNCTIONS

#include "Carbonate/AEDataModel.hh"

static inline
short mem_error()
{
	return *(short*) 0x220;  // MemErr
}

// These functions are always declared in the headers and are always extern.

// The comments in AEDataModel.h are wrong -- these functions ARE in CarbonAccessors.o.

extern "C" Boolean AECheckIsRecord( const AEDesc* desc )
{
	return desc->descriptorType == typeAERecord;  // FIXME:  Need to reverse-engineer format
}

extern "C" Size AESizeOfFlattenedDesc( const AEDesc* desc )
{
	return 0;
}

extern "C" OSStatus AEFlattenDesc( const AEDesc*  desc,
                                   Ptr            buffer,
                                   Size           bufferSize,
                                   Size*          actualSize )
{
	return paramErr;
}

extern "C" OSStatus AEUnflattenDesc( Ptr buffer, AEDesc* result )
{
	return paramErr;
}


/*
	These functions are declared either in Universal Interfaces (if ACCESSOR_CALLS_ARE_FUNCTIONS) or
	Carbonate otherwise.  They're implemented in CarbonAccessors.o for PPC.
*/

pascal OSErr AEGetDescData( const AEDesc*  desc,
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

pascal Size AEGetDescDataSize( const AEDesc* desc )
{
	if ( desc->descriptorType == typeNull )
	{
		return 0;
	}
	
	return GetHandleSize( desc->dataHandle );
}

pascal OSErr AEReplaceDescData( DescType     typeCode,
                                const void*  dataPtr,
                                Size         dataSize,
                                AEDesc*      desc )
{
	bool typeIsNull = typeCode == typeNull;
	
	bool descWasNull = desc->dataHandle == NULL;
	
	if ( !descWasNull  &&  *desc->dataHandle == NULL )
	{
		// empty handle
		DisposeHandle( desc->dataHandle );
		
		descWasNull = true;
	}
	
	if ( !descWasNull && !typeIsNull )
	{
		// Replace the data.  Resize the handle, copy the data, and set the type.
		SetHandleSize( desc->dataHandle, dataSize );
		
		if ( mem_error() != noErr )
		{
			return mem_error();
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

#endif
