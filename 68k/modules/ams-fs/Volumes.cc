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

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "c_string.hh"


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
	
	UInt8 name_len = *name++;
	
	if ( name[ name_len - 1 ] == ':' )
	{
		--name_len;
	}
	
	while ( vcb  &&  EqualString_sans_case( name, name_len, vcb->vcbVN ) )
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

static const char* wdnea = " (which doesn't exist anyway)";

short UnmountVol_patch( short trap_word : __D1, VolumeParam* pb : __A0 )
{
	StringPtr name = pb->ioNamePtr;
	
	VCB* vcb = NULL;
	
	if ( name )
	{
		// ioNamePtr
		vcb = VCB_lookup( name );
		
		WARNING = "Ignoring UnmountVol of ", CSTR( name ), vcb ? "" : wdnea;
	}
	else if ( pb->ioVRefNum )
	{
		// ioVRefNum
		vcb = VCB_lookup( pb->ioVRefNum );
		
		WARNING = "Ignoring UnmountVol of ", pb->ioVRefNum, vcb ? "" : wdnea;
	}
	
	if ( vcb == NULL )
	{
		return pb->ioResult = nsvErr;
	}
	
	return pb->ioResult = extFSErr;
}

short Eject_patch( short trap_word : __D1, VolumeParam* pb : __A0 )
{
	StringPtr name = pb->ioNamePtr;
	
	VCB* vcb = NULL;
	
	if ( name )
	{
		// ioNamePtr
		vcb = VCB_lookup( name );
		
		WARNING = "Ignoring Eject of ", CSTR( name ), vcb ? "" : wdnea;
	}
	else if ( pb->ioVRefNum )
	{
		// ioVRefNum
		vcb = VCB_lookup( pb->ioVRefNum );
		
		WARNING = "Ignoring Eject of ", pb->ioVRefNum, vcb ? "" : wdnea;
	}
	
	if ( vcb == NULL )
	{
		return pb->ioResult = nsvErr;
	}
	
	return pb->ioResult = extFSErr;
}
