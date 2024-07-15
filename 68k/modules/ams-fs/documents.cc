/*
	documents.cc
	------------
*/

#include "documents.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <errno.h>
#include <stddef.h>

// ams-common
#include "callouts.hh"
#include "FCB.hh"
#include "module_A4.hh"
#include "scoped_zone.hh"

// ams-fs
#include "freemount.hh"
#include "mount.hh"
#include "remotefs.hh"


#define STRLEN( s )      (sizeof "" s - 1)
#define STR_LEN( s )  s, (sizeof "" s - 1)

#define SETFINFO "..namedfork/SetFInfo"


short SFSaveDisk : 0x0214;

int docfs_fd;

void mount_virtual_documents_volume()
{
	static VCB virtual_documents_volume_VCB;
	
	VCB* vcb = &virtual_documents_volume_VCB;
	
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

static plus::var_string filename_cache;

const uint8_t* documents_get_nth( VCB* vcb, short n )
{
	const int in  = docfs_fd;
	const int out = docfs_fd;
	
	return remotefs_get_nth( in, out, n, filename_cache );
}

OSErr documents_Close( FCB* fcb )
{
	Handle h = (Handle) fcb->fcbFlNum;
	
	DisposeHandle( h );
	
	return noErr;
}

OSErr documents_Write( FCB* fcb, const char* buffer, uint32_t length )
{
	temp_A4 a4;
	
	StringPtr p = *(StringHandle) fcb->fcbFlNum;
	
	plus::string name( p,              vxo::delete_never );
	plus::string data( buffer, length, vxo::delete_never );
	
	int err = try_to_write( docfs_fd, name, data, fcb->fcbCrPs );
	
	return err ? ioErr : noErr;
}

OSErr documents_Create( VCB* vcb, const uint8_t* name )
{
	int err;
	
	temp_A4 a4;
	
	plus::var_string file_data;
	
	err = try_to_get( docfs_fd, name, file_data );
	
	if ( ! err )
	{
		return dupFNErr;
	}
	
	if ( err != -ENOENT )
	{
		return ioErr;
	}
	
	err = try_to_put( docfs_fd, name, plus::string::null );
	
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
	
	const int path_len = name[ 0 ] + !! is_rsrc * STRLEN( "/..namedfork/rsrc" );
	
	Str255 path;
	
	unsigned char* p = path;
	
	*p++ = path_len;
	
	p = (unsigned char*) fast_mempcpy( p, name + 1, name[ 0 ] );
	
	if ( is_rsrc )
	{
		fast_mempcpy( p, STR_LEN( "/..namedfork/rsrc" ) );
	}
	
	temp_A4 a4;
	
	plus::var_string file_data;
	
	int err = try_to_get( docfs_fd, path, file_data );
	
	if ( err < 0 )
	{
		// TODO:  Check for other errors.
		return fnfErr;
	}
	
	scoped_zone null_zone;
	
	const size_t size = file_data.size();
	
	StringHandle h = PtrToHand( path, 1 + path[ 0 ] );
	
	fcb->fcbFlNum  = (long) h;
	
	fcb->fcbTypByt = 0;
	fcb->fcbSBlk   = 0;
	fcb->fcbEOF    = size;
	fcb->fcbPLen   = size;
	
	fcb->fcbBfAdr  = NewPtr( size );
	
	fast_memcpy( fcb->fcbBfAdr, file_data.data(), size );
	
	return noErr;
}

OSErr documents_FlushFile( FCB* fcb )
{
	temp_A4 a4;
	
	StringPtr p = *(StringHandle) fcb->fcbFlNum;
	
	plus::string name( p,                           vxo::delete_never );
	plus::string data( fcb->fcbBfAdr, fcb->fcbPLen, vxo::delete_never );
	
	int err = try_to_put( docfs_fd, name, data );
	
	return err ? ioErr : noErr;
}

OSErr documents_GetFileInfo( HFileParam* pb, const uint8_t* name )
{
	const int in  = docfs_fd;
	const int out = docfs_fd;
	
	return remotefs_GetFileInfo( in, out, pb, name );
}

OSErr documents_SetFileInfo( HFileParam* pb, const uint8_t* name )
{
	temp_A4 a4;
	
	Size len = *name++;
	
	if ( len  &&  *name == ':' )
	{
		++name;
		--len;
	}
	
	char path[ 256 + STRLEN( "/" SETFINFO ) ];
	
	fast_memcpy( path, name, len );
	fast_memcpy( path + len, STR_LEN( "/" SETFINFO ) );
	
	plus::string pathname( path, len + STRLEN( "/" SETFINFO ) );
	
	const Size size = sizeof (FileParam) - offsetof( FileParam, ioFlFndrInfo );
	
	plus::string file_info( (char*) &pb->ioFlFndrInfo, size );
	
	int err = try_to_put( docfs_fd, pathname, file_info );
	
	if ( err == -ENOENT )
	{
		return fnfErr;
	}
	
	if ( err )
	{
		return ioErr;
	}
	
	return noErr;
}
