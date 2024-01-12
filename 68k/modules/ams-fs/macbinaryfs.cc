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

// ams-fs
#include "Volumes.hh"


static
const macbinary::header* next( const macbinary::header* h )
{
	uint32_t forkLen;
	
	const uint32_t data_offset = 128 + h->extHeaderLen + 127 & ~0x7f;
	
	fast_memcpy( &forkLen, h->dataForkLen, 4 );
	
	const uint32_t rsrc_offset = data_offset + forkLen + 127 & ~0x7f;
	
	fast_memcpy( &forkLen, h->rsrcForkLen, 4 );
	
	const uint32_t next_offset = rsrc_offset + forkLen + 127 & ~0x7f;
	
	h = (const macbinary::header*) ((Ptr) h + next_offset);
	
	return h;
}

const macbinary::header* MacBinary_get_nth( VCB* vcb, short n )
{
	const macbinary::header* h = (const macbinary::header*) vcb->vcbBufAdr;
	
	uint16_t depth = 0;
	
	do
	{
		if ( h->extensions == 0 )
		{
			if ( --n <= 0 )
			{
				return h;
			}
		}
		else
		{
			depth += (int8_t) h->fileCreator[ 3 ] * 2 + 3;
		}
		
		h = next( h );
	}
	while ( depth > 0 );
	
	return NULL;
}

const macbinary::header* MacBinary_lookup( VCB* vcb, const uint8_t* name )
{
	if ( name == NULL  ||  name[ 0 ] == '\0' )
	{
		return NULL;
	}
	
	uint8_t name_len = *name++;
	
	if ( *name == ':' )
	{
		++name;
		--name_len;
	}
	
	const macbinary::header* h = (const macbinary::header*) vcb->vcbBufAdr;
	
	uint16_t depth = 0;
	
	do
	{
		if ( h->extensions == 0 )
		{
			const uint8_t* this_name = h->filename;
			const uint8_t  this_len  = *this_name++;
			
			if ( EqualString_sans_case( this_name, this_len, name, name_len ) )
			{
				return h;
			}
		}
		else
		{
			depth += (int8_t) h->fileCreator[ 3 ] * 2 + 3;
		}
		
		h = next( h );
	}
	while ( depth > 0 );
	
	return NULL;
}

OSErr MacBinary_Close( FCB* fcb )
{
	fcb->fcbBfAdr = NULL;  // Don't let Close() dispose of the volume buffer
	
	return noErr;
}

static inline
uint32_t get_filenum( VCB* vcb, const macbinary::header* h )
{
	return h - (const macbinary::header*) vcb->vcbBufAdr + 1;
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
	
	fcb->fcbFlNum  = get_filenum( fcb->fcbVPtr, h );
	fcb->fcbTypByt = 0;
	fcb->fcbSBlk   = fcb->fcbPLen >> 7;
	
	return noErr;
}

OSErr MacBinary_GetFileInfo( HFileParam* pb, const macbinary::hdr* h )
{
	pb->ioFlAttrib  = 0;
	pb->ioFlVersNum = 0;
	
	fast_memcpy( &pb->ioFlFndrInfo, h->fileType, sizeof (FInfo) );
	
	((uint8_t*) &pb->ioFlFndrInfo.fdFlags)[ 1 ] = h->fndrFlagsLow;
	
	pb->ioDirID = get_filenum( VCB_lookup( pb->ioVRefNum ), h );  // ioFlNum
	
	pb->ioFlStBlk  = 0;
	pb->ioFlRStBlk = 0;
	
	fast_memcpy( &pb->ioFlLgLen,  h->dataForkLen, 4 );
	fast_memcpy( &pb->ioFlRLgLen, h->rsrcForkLen, 4 );
	
	pb->ioFlPyLen  = (pb->ioFlLgLen  + 127) & ~0x7f;
	pb->ioFlRPyLen = (pb->ioFlRLgLen + 127) & ~0x7f;
	
	fast_memcpy( &pb->ioFlCrDat, h->crDate, 4 );
	fast_memcpy( &pb->ioFlMdDat, h->mdDate, 4 );
	
	if ( pb->ioFDirIndex > 0  &&  pb->ioNamePtr )
	{
		ConstStr255Param name = h->filename;
		
		fast_memcpy( pb->ioNamePtr, name, 1 + name[ 0 ] );
	}
	
	return noErr;
}
