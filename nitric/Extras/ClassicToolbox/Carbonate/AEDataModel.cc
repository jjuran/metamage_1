// Carbonate/AEDataModel.cc


#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#if !TARGET_API_MAC_CARBON

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
#include "Carbonate/AEDataModel.hh"
#endif

// These functions are always declared in the headers and are always extern.

// The comments in AEDataModel.h are wrong -- these functions ARE in CarbonAccessors.o.

#if TARGET_CPU_68K

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

#endif

#endif

