// AESubDescs_CPlus.h
//
// A high-efficiency way to examine AEDescs. Everything is done in place, without any
// copying of data, which avoids most of the overhead of the Apple Event Manager.
//
// By Jens Alfke; Copyright ©1992 Apple Computer. All Rights Reserved.


#ifdef THINK_C
#pragma once										/* For THINK C users */
#endif

#ifndef __AESUBDESCS__
#define __AESUBDESCS__								/* For poor MPW users :) */

#include <AppleEvents.h>

enum{												// Error code
	errAEListIsFactored		= -1760						// I cannot get data from factored lists
};


struct AESubDesc;


extern "C" {
	pascal void
		AEDescToSubDesc( const AEDesc*, AESubDesc* );					// Create subDesc on desc
	pascal OSErr
		AESubDescToDesc( const AESubDesc*, long desiredType, AEDesc* );	// Copy subDesc to new desc
	
	pascal DescType
		AEGetSubDescType( const AESubDesc* );							// Same as ->subDescType
	pascal void*
		AEGetSubDescData( const AESubDesc*, long *length );				// Invalid once dataHandle moves
	
	pascal Boolean
		AESubDescIsListOrRecord( const AESubDesc* );	// Is it a list or (possibly coerced) record?
	pascal DescType
		AEGetSubDescBasicType( const AESubDesc* );		// Returns 'reco' if it's a coerced record
	
	// The list-oriented calls that follow make sure the subdescriptor is a valid list or (possibly
	// coerced) record. If not, they'll return errAEWrongDataType.
	
	pascal long
		AECountSubDescItems( const AESubDesc* );
	
	// In these next two calls, it's okay if newSD == sd; sd will be overwritten with the new subDesc.
		
	pascal OSErr
		AEGetNthSubDesc( const AESubDesc* sd, long index,
						 AEKeyword* keyIfAny = NULL, AESubDesc* newSD ),
		AEGetKeySubDesc( const AESubDesc* sd, AEKeyword,				// Lists illegal here
						 AESubDesc* newSD );
};


struct AESubDesc {
	public:
	
	AESubDesc( const AEDesc &desc )				{AEDescToSubDesc(desc,this);}
	
	DescType	GetType( void ) const			{return subDescType;}
	DescType	GetBasicType( void ) const		{return AEGetSubDescType(this);}
	Boolean		IsListOrRecord( void ) const	{return AESubDescIsListOrRecord(this);}
	
	const void*	GetData( long *length =NULL ) const
												{return AEGetSubDescData(length);}
	long		GetDataLength( void ) const		{long length; AEGetSubDescData(&length); return length;}
	
	long		CountItems( void ) const		{return AECountSubDescItems(this);}
	
	OSErr		GetNth( long index, AESubDesc *newSD, AEKeyword *keyIfAny =NULL ) const
												{return AEGetNthSubDesc(this,index,keyIfAny,newSD);}
	OSErr		GetKey( AEKeyword key, AESubDesc *newSD ) const
												{return AEGetKeySubDesc(this,key,newSD);}

	private:
	DescType	subDescType;		// Type of this subDesc. You may read this field.
	Handle		dataHandle;			// Handle to main (outer) descriptor. Private.
	long		offset;				// Offset into main descriptor where subDesc starts. Private.
};


#endif