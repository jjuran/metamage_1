////
////	AEStream_CPlus.h		A (write-only) stream for creating AE Descriptors.
////							This header file for use with C++. Use AEStream.h with C.
////
////	By Jens Alfke			©1991-1992 Apple Computer, Inc. All rights reserved.
////


// NOTE: In case of disagreement between this header and the C one (AEStream.h),
//		 the C header is correct and this header needs to be fixed.


#ifndef __AESTREAM__
	#define __AESTREAM__

#ifndef __MEMORY__
	#include <Memory.h>
#endif
#ifndef __APPLEEVENTS__
	#include <AppleEvents.h>
#endif

const errAEStream_BadNesting	= 13579;	// Bad descriptor/array nesting error

//	Here are the C-style definitions, which are the actual functions implemented:

struct AEStream;

extern "C" {
	OSErr
		AEStream_Open		( AEStream* ),
		AEStream_Close		( AEStream*, AEDesc *desc =NULL ),

		AEStream_WriteDesc	( AEStream*, DescType type, const void *data, Size length ),
		AEStream_WriteAEDesc( AEStream*, const AEDesc &desc ),

		AEStream_OpenDesc	( AEStream*, DescType type ),
		AEStream_WriteData	( AEStream*, const void *data, Size length ),
		AEStream_CloseDesc	( AEStream* ),

		AEStream_OpenList	( AEStream* ),
		AEStream_CloseList	( AEStream* ),

		AEStream_OpenRecord	( AEStream*, DescType type ),
		AEStream_SetRecordType( AEStream* s, DescType type ),
		AEStream_CloseRecord( AEStream* ),

		AEStream_WriteKeyDesc(AEStream*, AEKeyword key,
											DescType type, const void *data, Size length ),
		AEStream_OpenKeyDesc( AEStream*, AEKeyword key, DescType type ),
		AEStream_WriteKey	( AEStream*, AEKeyword key ),

		AEStream_CreateEvent( AEStream*, AEEventClass, AEEventID,
								DescType targetType, const void *targetData, long targetLength,
								short returnID =kAutoGenerateReturnID,
								long transactionID =kAnyTransactionID ),
		AEStream_OpenEvent	( AEStream*, AppleEvent& ),
		AEStream_OptionalParam( AEStream*, AEKeyword key );
}

// Here are the data structures, complete with fancy C++ inline methods to call the above fns:


struct AEStream {		// A (write-only) stream on an AE descriptor
	public:
	
	inline AEStream( )							{AEStream_Open(this);}
	inline AEStream( AppleEvent &aevt )			{AEStream_OpenEvent(this,aevt);}
	inline AEStream( AEEventClass Class, AEEventID id,
					 DescType targetType, const void *targetData, long targetLength,
					 short returnID =kAutoGenerateReturnID,
					 long transactionID =kAnyTransactionID )
					 							{AEStream_CreateEvent(this,Class,id,
					 									targetType,targetData,targetLength,
					 									returnID,transactionID);}
	inline ~AEStream()							{AEStream_Close(this);}
	// I'm having doubts about these constructors, since constructors can't
	// return error results .... Hmm?
	
	inline OSErr
		Close		( AEDesc *desc =NULL )
												{return AEStream_Close(this,desc);}

		OpenDesc	( DescType type )
												{return AEStream_OpenDesc(this,type);}
		WriteData	( const void *data, Size length )
												{return AEStream_WriteData(this,data,length);}
		CloseDesc	( void )
												{return AEStream_CloseDesc(this);}

		WriteDesc	( DescType type, const void *data, Size length )
												{return AEStream_WriteDesc(this,type,data,length);}
		WriteDesc	( const AEDesc &desc )
												{return AEStream_WriteAEDesc(this,desc);}

		OpenList	( void )
												{return AEStream_OpenList(this);}
		CloseList	( void )
												{return AEStream_CloseList(this);}

		OpenRecord	( DescType type )
												{return AEStream_OpenRecord(this,type);}
		SetRecordType( DescType type )
												{return AEStream_SetRecordType(this,type);}
		CloseRecord	( void )
												{return AEStream_CloseRecord(this);}

		WriteKeyDesc( AEKeyword key, DescType type, void *data, Size length )
										{return AEStream_WriteKeyDesc(this,key,type,data,length);}
		OpenKeyDesc	( AEKeyword key, DescType type )
												{return AEStream_OpenKeyDesc(this,key,type);}
		WriteKey	( AEKeyword key )
												{return AEStream_WriteKey(this,key);}
	private:
		Handle	data;					// The data
		Size	size;					// Current size of handle [same as GetHandleSize(data)]
		Size	index;					// Current index (into data handle) to write to
		Size	headIndex;				// Offset of header [type field] of open descriptor
		char	context;				// What context am I in? [enum]
		char	isEvent;				// Is this an Apple Event? [Boolean]
};


#endif