/*
	bootstrap.cc
	------------
*/

#include "bootstrap.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <errno.h>

// ams-common
#include "callouts.hh"
#include "FCB.hh"
#include "module_A4.hh"

// ams-fs
#include "freemount.hh"
#include "mount.hh"


void mount_virtual_bootstrap_volume()
{
	static VCB virtual_bootstrap_volume_VCB;
	
	VCB* vcb = &virtual_bootstrap_volume_VCB;
	
	fast_memset( vcb, '\0', sizeof (VCB) );
	
	vcb->vcbSigWord = 'Ix';
	vcb->vcbFSID    = 'Ix';
	
	vcb->vcbAtrb |= kioVAtrbHardwareLockedMask;
	
	#define VOLNAME  "\p" "Bootstrap"
	
	fast_memcpy( vcb->vcbVN, VOLNAME, sizeof VOLNAME );
	
	#undef VOLNAME
	
	mount_VCB( vcb );
}

const uint8_t* bootstrap_get_nth( VCB* vcb, short n )
{
	return NULL;  // not yet implemented
}

const uint8_t* bootstrap_lookup( VCB* vcb, const uint8_t* name )
{
	if ( name == NULL  ||  name[ 0 ] == '\0' )
	{
		return NULL;
	}
	
	return name;
}

OSErr bootstrap_open_fork( short trap_word, FCB* fcb, const uint8_t* name )
{
	const Byte is_rsrc = trap_word;  // Open is A000, OpenRF is A00A
	
	temp_A4 a4;
	
	Size len = name[ 0 ];
	
	char path[ 256 + 5 ];
	
	fast_memcpy( path, name + 1, len );
	path[ len ] = '\0';
	
	plus::var_string file_data;
	
	int err;
	
	if ( ! is_rsrc )
	{
		err = try_to_get( path, len, file_data );
		
		if ( err == -EISDIR )
		{
			fast_memcpy( path + len, "/data", 5 );
			
			err = try_to_get( path, len + 5, file_data );
		}
	}
	else
	{
		fast_memcpy( path + len, "/rsrc", 5 );
		
		err = try_to_get( path, len + 5, file_data );
	}
	
	if ( err < 0 )
	{
		return fnfErr;  // TODO:  Check for other errors.
	}
	
	const Size size = file_data.size();
	
	Ptr buffer = NewPtr( size );
	
	if ( buffer == NULL )
	{
		return memFullErr;
	}
	
	BlockMoveData( file_data.data(), buffer, size );
	
	fcb->fcbFlNum = -1;  // Claim the FCB as in use.
	
	fcb->fcbTypByt = 0;
	fcb->fcbSBlk   = 0;
	fcb->fcbEOF    = size;
	fcb->fcbPLen   = size;
	
	fcb->fcbBfAdr = (Ptr) buffer;
	
	return noErr;
}

OSErr bootstrap_GetFileInfo( FileParam* pb, const uint8_t* name )
{
	return extFSErr;
}
