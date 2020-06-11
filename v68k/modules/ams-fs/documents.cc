/*
	documents.cc
	------------
*/

#include "documents.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif

// Standard C
#include <errno.h>

// ams-common
#include "callouts.hh"
#include "FCB.hh"
#include "module_A4.hh"
#include "scoped_zone.hh"

// ams-fs
#include "freemount.hh"
#include "mount.hh"


short SFSaveDisk : 0x0214;

int docfs_fd;

void mount_virtual_documents_volume()
{
	static VCB virtual_documents_volume_VCB;
	
	VCB* vcb = &virtual_documents_volume_VCB;
	
	fast_memset( vcb, '\0', sizeof (VCB) );
	
	vcb->vcbSigWord = 0xD0C5;
	vcb->vcbFSID    = 0xD0C5;
	
	// 16 MiB free should be sufficient for now
	vcb->vcbAlBlkSiz = 4096;
	vcb->vcbFreeBks  = 4096;
	
	#define VOLNAME  "\p" "Documents"
	
	fast_memcpy( vcb->vcbVN, VOLNAME, sizeof VOLNAME );
	
	#undef VOLNAME
	
	const short vRefNum = mount_VCB( vcb );
	
	SFSaveDisk = -vRefNum;
}

const uint8_t* documents_get_nth( VCB* vcb, short n )
{
	return NULL;  // not yet implemented
}

const uint8_t* documents_lookup( VCB* vcb, const uint8_t* name )
{
	if ( name == NULL  ||  name[ 0 ] == '\0' )
	{
		return NULL;
	}
	
	return name;
}

OSErr documents_Close( FCB* fcb )
{
	Handle h = (Handle) fcb->fcbFlNum;
	
	DisposeHandle( h );
	
	return noErr;
}

OSErr documents_Create( VCB* vcb, const uint8_t* name )
{
	temp_A4 a4;
	
	int err = try_to_put( docfs_fd, name, plus::string::null );
	
	return err ? ioErr : noErr;
}

enum
{
	_PtrToHand = 0xA9E3,
};

static inline asm
StringHandle PtrToHand( const void* p : __A0, UInt32 n : __D0 )
{
	_PtrToHand
}

OSErr documents_open_fork( short trap_word, FCB* fcb, const uint8_t* name )
{
	const Byte is_rsrc = trap_word;  // Open is A000, OpenRF is A00A
	
	if ( is_rsrc )
	{
		return extFSErr;
	}
	
	temp_A4 a4;
	
	plus::var_string file_data;
	
	int err = try_to_get( docfs_fd, name, file_data );
	
	if ( ! err )
	{
		scoped_zone null_zone;
		
		const size_t size = file_data.size();
		
		StringHandle h = PtrToHand( name, 1 + name[ 0 ] );
		
		fcb->fcbFlNum  = (long) h;
		
		fcb->fcbTypByt = 0;
		fcb->fcbSBlk   = 0;
		fcb->fcbEOF    = size;
		fcb->fcbPLen   = size;
		
		fcb->fcbBfAdr  = NewPtr( size );
		
		fast_memcpy( fcb->fcbBfAdr, file_data.data(), size );
	}
	
	return noErr;
}

OSErr documents_FlushFile( FCB* fcb )
{
	temp_A4 a4;
	
	StringPtr p = *(StringHandle) fcb->fcbFlNum;
	
	plus::string name( p,                           plus::delete_never );
	plus::string data( fcb->fcbBfAdr, fcb->fcbPLen, plus::delete_never );
	
	int err = try_to_put( docfs_fd, name, data );
	
	return err ? ioErr : noErr;
}

OSErr documents_GetFileInfo( FileParam* pb, const uint8_t* name )
{
	return noErr;
}
