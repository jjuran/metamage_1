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

// ams-common
#include "callouts.hh"

// ams-fs
#include "freemount.hh"
#include "macbinary.hh"


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
	
	Ptr image = NewPtr( data_size );
	
	if ( image == NULL )
	{
		return;
	}
	
	VCB* vcb = (VCB*) NewPtrClear( sizeof (VCB) );
	
	if ( vcb == NULL )
	{
		DisposePtr( image );
		return;
	}
	
	fast_memcpy( image, data.data(), data_size );
	
	vcb->qType    = fsQType;
	vcb->vcbFlags = 0;
	
	const macbinary::hdr& possible_mBIN_header = *(const macbinary::hdr*) image;
	
	if ( int8_t version = macbinary::version( possible_mBIN_header ) )
	{
		vcb->vcbSigWord = 'mB';
		
		const uint8_t* filename = possible_mBIN_header.filename;
		
		uint8_t len = filename[ 0 ];
		
		if ( len > 27 )
		{
			len = 27;
		}
		
		vcb->vcbVN[ 0 ] = len;
		
		fast_memcpy( vcb->vcbVN + 1, filename + 1, len );
	}
	else
	{
		Ptr master_directory_block = image + 1024;
		
		fast_memcpy( &vcb->vcbSigWord, master_directory_block, 64 );
		
		if ( vcb->vcbSigWord == 0xD2D7 )
		{
			vcb->vcbMAdr = master_directory_block + 64;
			vcb->vcbMLen = (vcb->vcbNmAlBlks * 12 + 7) / 8u;
		}
	}
	
	vcb->vcbAtrb |= kioVAtrbHardwareLockedMask;
	
	vcb->vcbMAdr   = NULL;
	vcb->vcbBufAdr = image;
	vcb->vcbMLen   = 0;
	
	mount_VCB( vcb );
}
