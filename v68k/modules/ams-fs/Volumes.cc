/*
	Volumes.cc
	----------
*/

#include "Volumes.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// ams-common
#include "callouts.hh"


#define VOLNAME  "\p" "AMS Made-up Storage"


enum
{
	kHFSFlagMask = 0x0200,
};

VCB* DefVCBPtr : 0x0352;
QHdr VCBQHdr   : 0x0356;

static inline
void fast_memcpy( void* dst, const void* src, UInt32 n )
{
	BlockMoveData( src, dst, n );
}

static VCB virtual_network_volume_VCB;

void mount_virtual_network_volume()
{
	VCB* vcb = &virtual_network_volume_VCB;
	
	fast_memset( vcb, '\0', sizeof (VCB) );
	
	vcb->vcbSigWord = 'Ix';
	vcb->vcbFSID    = 'Ix';
	vcb->vcbVRefNum = -1;
	
	fast_memcpy( vcb->vcbVN, VOLNAME, sizeof VOLNAME );
	
	Enqueue( (QElemPtr) vcb, &VCBQHdr );
	
	DefVCBPtr = vcb;
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

short FlushVol_patch( short trap_word : __D1, VolumeParam* pb : __A0 )
{
	return pb->ioResult = noErr;
}
