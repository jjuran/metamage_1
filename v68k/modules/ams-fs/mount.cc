/*
	mount.cc
	--------
*/

#include "mount.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <string.h>

// ams-fs
#include "freemount.hh"


VCB* DefVCBPtr : 0x0352;
QHdr VCBQHdr   : 0x0356;

short mount_VCB( VCB* vcb )
{
	static short last_vRefNum;
	
	vcb->vcbVRefNum = --last_vRefNum;
	
	Enqueue( (QElemPtr) vcb, &VCBQHdr );
	
	if ( DefVCBPtr == NULL )
	{
		DefVCBPtr = vcb;
	}
	
	return last_vRefNum;
}

void try_to_mount( const char* name )
{
	/*
		Don't save/restore A4 here.  Currently, we're only called before
		module_A4_suspend(), so our A4 is already active (and "restoring" it
		would be harmful).  If/when mounting is allowed later, the new caller
		can save/restore A4 on its own.
	*/
	
	plus::var_string data;
	
	int nerr = try_to_get( name, strlen( name ), data );
	
	if ( nerr )
	{
		return;
	}
	
	const size_t data_size = data.size();
	
	Ptr image = NewPtrSys( data_size );
	
	if ( image == NULL )
	{
		return;
	}
	
	VCB* vcb = (VCB*) NewPtr( sizeof (VCB) );
	
	if ( vcb == NULL )
	{
		DisposePtr( image );
		return;
	}
	
	BlockMoveData( data.data(), image, data_size );
	
	Ptr master_directory_block = image + 1024;
	
	vcb->qType    = fsQType;
	vcb->vcbFlags = 0;
	
	BlockMoveData( master_directory_block, &vcb->vcbSigWord, 64 );
	
	vcb->vcbMAdr   = NULL;
	vcb->vcbBufAdr = image;
	vcb->vcbMLen   = 0;
	
	const uint16_t sigword = vcb->vcbSigWord;
	
	if ( sigword == 0xD2D7 )
	{
		vcb->vcbMAdr = master_directory_block + 64;
		vcb->vcbMLen = (vcb->vcbNmAlBlks * 12 + 7) / 8u;
	}
	
	mount_VCB( vcb );
}
