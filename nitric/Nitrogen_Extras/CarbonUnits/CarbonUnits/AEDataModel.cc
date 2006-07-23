// CarbonUnits/AEDataModel.cc


#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONUNITS_LINKAGE pascal
#include "CarbonUnits/AEDataModel.hh"
#endif

// These functions are always declared in the headers and are always extern.

// Search AEDataModel.h for 'Non-Carbon CFM:   not available'

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

