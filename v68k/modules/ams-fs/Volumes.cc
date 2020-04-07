/*
	Volumes.cc
	----------
*/

#include "Volumes.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif

// ams-common
#include "callouts.hh"


enum
{
	kHFSFlagMask = 0x0200,
};

VCB* DefVCBPtr : 0x0352;
QHdr VCBQHdr   : 0x0356;

VCB* VCB_lookup( short whichVol )
{
	if ( whichVol == 0 )
	{
		return DefVCBPtr;
	}
	
	VCB* vcb = (VCB*) VCBQHdr.qHead;
	
	if ( whichVol < 0 )
	{
		while ( vcb != NULL  &&  vcb->vcbVRefNum != whichVol )
		{
			vcb = (VCB*) vcb->qLink;
		}
	}
	else
	{
		while ( vcb != NULL  &&  --whichVol )
		{
			vcb = (VCB*) vcb->qLink;
		}
	}
	
	return vcb;
}

VCB* VCB_lookup( const unsigned char* name )
{
	VCB* vcb = (VCB*) VCBQHdr.qHead;
	
	while ( vcb != NULL  &&  EqualString_sans_case( vcb->vcbVN, name ) )
	{
		vcb = (VCB*) vcb->qLink;
	}
	
	return vcb;
}

short GetVolInfo_patch( short trap_word : __D1, VolumeParam* pb : __A0 )
{
	VCB* vcb = NULL;
	
	if ( pb->ioVolIndex > 0 )
	{
		// nth volume
		vcb = VCB_lookup( pb->ioVolIndex );
	}
	else if ( pb->ioVolIndex < 0  &&  pb->ioNamePtr != NULL )
	{
		// ioNamePtr
		vcb = VCB_lookup( pb->ioNamePtr );
	}
	else
	{
		// ioVRefNum
		vcb = VCB_lookup( pb->ioVRefNum );
	}
	
	if ( vcb == NULL )
	{
		return pb->ioResult = nsvErr;
	}
	
	if ( pb->ioNamePtr )
	{
		ConstStr255Param name = vcb->vcbVN;
		
		fast_memcpy( pb->ioNamePtr, name, 1 + name[ 0 ] );
	}
	
	pb->ioVRefNum = vcb->vcbVRefNum;
	
	const size_t n = offsetof( VCB, vcbVN ) - offsetof( VCB, vcbCrDate );
	
	fast_memcpy( &pb->ioVCrDate, &vcb->vcbCrDate, n );
	
	return pb->ioResult = noErr;
}

short GetVol_patch( short trap_word : __D1, WDPBRec* pb : __A0 )
{
	if ( DefVCBPtr == NULL )
	{
		return pb->ioResult = nsvErr;
	}
	
	const short vRefNum = DefVCBPtr->vcbVRefNum;
	
	if ( trap_word & kHFSFlagMask )
	{
		pb->ioWDProcID  = 0;
		pb->ioWDVRefNum = vRefNum;
		pb->ioWDDirID   = 2;
	}
	
	if ( pb->ioNamePtr )
	{
		StringPtr name = DefVCBPtr->vcbVN;
		
		fast_memcpy( pb->ioNamePtr, name, 1 + name[ 0 ] );
	}
	
	pb->ioVRefNum = vRefNum;
	
	return pb->ioResult = noErr;
}

short SetVol_patch( short trap_word : __D1, WDPBRec* pb : __A0 )
{
	VCB* vcb = NULL;
	
	if ( pb->ioNamePtr )
	{
		// ioNamePtr
		vcb = VCB_lookup( pb->ioNamePtr );
	}
	else
	{
		// ioVRefNum
		vcb = VCB_lookup( pb->ioVRefNum );
	}
	
	if ( vcb == NULL )
	{
		return pb->ioResult = nsvErr;
	}
	
	DefVCBPtr = vcb;
	
	return pb->ioResult = noErr;
}

short FlushVol_patch( short trap_word : __D1, VolumeParam* pb : __A0 )
{
	return pb->ioResult = noErr;
}
