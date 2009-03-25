////
////	AEStream.h				A (write-only) stream for creating AE Descriptors.
////
////	By Jens Alfke			©1991-1992 Apple Computer, Inc. All rights reserved.
////


#pragma once

#ifndef __AESTREAM__
#define __AESTREAM__								/* For poor MPW users :) */

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

// The "AEStream_CPlus.h" header is no longer automatically included. You can include it
// yourself instead of this one if you want to use the object-based syntax in C++.


#define errAEStream_BadNesting	13579	/* Bad descriptor/array nesting error */


typedef struct {				// A (write-only) stream on an AE descriptor
	Handle	data;					// The data
	Size	size;					// Current size of handle [same as GetHandleSize(data)]
	Size	index;					// Current index (into data handle) to write to
	Size	headIndex;				// Offset of header [type field] of open descriptor
	char	context;				// What context am I in? [enum]
	char	isEvent;				// Is this an Apple Event? [Boolean]
} AEStream, *AEGizmos_AEStreamRef;


OSErr	AEStream_Open		( AEGizmos_AEStreamRef ),
		AEStream_Close		( AEGizmos_AEStreamRef, AEDesc *desc ),

		AEStream_OpenDesc	( AEGizmos_AEStreamRef, DescType type ),
		AEStream_WriteData	( AEGizmos_AEStreamRef, const void *data, Size length ),
		AEStream_CloseDesc	( AEGizmos_AEStreamRef ),

		AEStream_WriteDesc	( AEGizmos_AEStreamRef, DescType type, const void *data, Size length ),
		AEStream_WriteAEDesc( AEGizmos_AEStreamRef, const AEDesc *desc ),

		AEStream_OpenList	( AEGizmos_AEStreamRef ),
		AEStream_CloseList	( AEGizmos_AEStreamRef ),

		AEStream_OpenRecord	( AEGizmos_AEStreamRef, DescType type ),
		AEStream_SetRecordType( AEGizmos_AEStreamRef s, DescType type ),
		AEStream_CloseRecord( AEGizmos_AEStreamRef ),

		AEStream_WriteKeyDesc(AEGizmos_AEStreamRef, AEKeyword key, DescType type, const void *data, Size length ),
		AEStream_OpenKeyDesc( AEGizmos_AEStreamRef, AEKeyword key, DescType type ),
		AEStream_WriteKey	( AEGizmos_AEStreamRef, AEKeyword key ),

		AEStream_CreateEvent( AEGizmos_AEStreamRef, AEEventClass, AEEventID,
								DescType targetType, const void *targetData, long targetLength,
								short returnID, long transactionID ),
		AEStream_OpenEvent	( AEGizmos_AEStreamRef, AppleEvent* ),
		AEStream_OptionalParam( AEGizmos_AEStreamRef, AEKeyword key );

#ifdef __cplusplus
}
#endif


#endif
