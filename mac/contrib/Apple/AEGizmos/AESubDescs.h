// AESubDescs.h
//
// A high-efficiency way to examine AEDescs. Everything is done in place, without any
// copying of data, which avoids most of the overhead of the Apple Event Manager.
//
// By Jens Alfke; Copyright ©1992-95 Apple Computer. All Rights Reserved.


#pragma once

#ifndef __AESUBDESCS__
#define __AESUBDESCS__								/* For poor MPW users :) */

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif


enum{												// Error code
	errAEListIsFactored		= -1760						// I cannot get data from factored lists
};


struct AESubDesc {
	DescType	subDescType;		// Type of this subDesc. You may read this field.
	Handle		dataHandle;			// Handle to main (outer) descriptor. Private.
	long		offset;				// Offset into main descriptor where subDesc starts. Private.
};

typedef struct AESubDesc AESubDesc;

#ifdef __cplusplus
extern "C" {
#endif

pascal void
	AEDescToSubDesc( const AEDesc*, AESubDesc* );					// Create subDesc on desc
pascal OSErr
	AESubDescToDesc( const AESubDesc*, long desiredType, AEDesc* );	// Copy subDesc to new desc

pascal DescType
	AEGetSubDescType( const AESubDesc* );							// Same as ->subDescType
pascal void*
	AEGetSubDescData( const AESubDesc*, long *length );				// Invalid once dataHandle moves
pascal void
	AECopySubDescData( const AESubDesc *sd, long *length, void *dst, long maxLen );
pascal Boolean
	AESubDescIsListOrRecord( const AESubDesc* sd );	// Is it a list or (possibly coerced) record?
pascal DescType
	AEGetSubDescBasicType( const AESubDesc* );		// Returns 'reco' if it's a coerced record

// The list-oriented calls that follow make sure the subdescriptor is a valid list or (possibly
// coerced) record. If not, they'll return errAEWrongDataType.

pascal long
	AECountSubDescItems( const AESubDesc* );

// In these next two calls, it's okay if newSD == sd; sd will be overwritten with the new subDesc.
	
pascal OSErr
	AEGetNthSubDesc( const AESubDesc* sd, long index,
					 AEKeyword* keyIfAny, AESubDesc* newSD ),
	AEGetKeySubDesc( const AESubDesc* sd, AEKeyword,				// Lists illegal here
					 AESubDesc* newSD );

#ifdef __cplusplus
}
#endif

#endif /*__AESUBDESCS__*/

