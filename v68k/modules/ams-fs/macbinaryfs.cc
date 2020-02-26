/*
	macbinaryfs.cc
	--------------
*/

#include "macbinaryfs.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif

// ams-common
#include "callouts.hh"
#include "FCB.hh"


const macbinary::header* MacBinary_get_nth( VCB* vcb, short n )
{
	if ( n == 1 )
	{
		return (const macbinary::header*) vcb->vcbBufAdr;
	}
	
	return NULL;
}

const macbinary::header* MacBinary_lookup( VCB* vcb, const uint8_t* name )
{
	if ( name == NULL  ||  name[ 0 ] == '\0' )
	{
		return NULL;
	}
	
	const macbinary::header* h = (const macbinary::header*) vcb->vcbBufAdr;
	
	if ( EqualString( h->filename, name, false, true ) )
	{
		return h;
	}
	
	return NULL;
}

OSErr MacBinary_Close( FCB* fcb )
{
	fcb->fcbBfAdr = NULL;  // Don't let Close() dispose of the volume buffer
	
	return noErr;
}

OSErr MacBinary_open_fork( short trap_word, FCB* fcb, const macbinary::hdr* h )
{
	const uint32_t data_offset = 128 + h->extHeaderLen + 127 & ~0x7f;
	
	fcb->fcbBfAdr = (Ptr) h + data_offset;
	
	fast_memcpy( &fcb->fcbEOF, h->dataForkLen, 4 );
	
	fcb->fcbPLen = (fcb->fcbEOF + 127) & ~0x7f;
	
	const Byte is_rsrc = trap_word;  // Open is A000, OpenRF is A00A
	
	if ( is_rsrc )
	{
		fcb->fcbBfAdr += fcb->fcbPLen;
		
		fast_memcpy( &fcb->fcbEOF, h->rsrcForkLen, 4 );
		
		fcb->fcbPLen = (fcb->fcbEOF + 127) & ~0x7f;
	}
	
	fcb->fcbFlNum  = 1;
	fcb->fcbTypByt = 0;
	fcb->fcbSBlk   = fcb->fcbPLen >> 7;
	
	return noErr;
}

OSErr MacBinary_GetFileInfo( FileParam* pb, const macbinary::hdr* h )
{
	pb->ioFlAttrib  = 0;
	pb->ioFlVersNum = 0;
	
	fast_memcpy( &pb->ioFlFndrInfo, h->fileType, sizeof (FInfo) );
	
	((uint8_t*) pb->ioFlFndrInfo.fdFlags)[ 1 ] = h->fndrFlagsLow;
	
	pb->ioFlNum = 1;
	
	pb->ioFlStBlk  = 0;
	pb->ioFlRStBlk = 0;
	
	fast_memcpy( &pb->ioFlLgLen,  h->dataForkLen, 4 );
	fast_memcpy( &pb->ioFlRLgLen, h->rsrcForkLen, 4 );
	
	pb->ioFlPyLen  = (pb->ioFlLgLen  + 127) & ~0x7f;
	pb->ioFlRPyLen = (pb->ioFlRLgLen + 127) & ~0x7f;
	
	fast_memcpy( &pb->ioFlCrDat, h->crDate, 4 );
	fast_memcpy( &pb->ioFlMdDat, h->mdDate, 4 );
	
	if ( pb->ioNamePtr )
	{
		ConstStr255Param name = h->filename;
		
		fast_memcpy( pb->ioNamePtr, name, 1 + name[ 0 ] );
	}
	
	return noErr;
}
