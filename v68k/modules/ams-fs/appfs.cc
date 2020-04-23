/*
	appfs.cc
	--------
*/

#include "appfs.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// ams-common
#include "callouts.hh"
#include "FCB.hh"
#include "module_A4.hh"
#include "scoped_zone.hh"

// ams-fs
#include "freemount.hh"


int appfs_fd;

static const plus::string data_path = "data";

void load_app_data( FCB* fcb )
{
	temp_A4 a4;
	
	plus::var_string existing_data;
	
	int err = try_to_get( appfs_fd, data_path, existing_data );
	
	if ( ! err  &&  ! existing_data.empty() )
	{
		const Size size = existing_data.size();
		
		if ( size > fcb->fcbPLen )
		{
			scoped_zone null_zone;
			
			DisposePtr( fcb->fcbBfAdr );
			
			fcb->fcbBfAdr = NewPtr( size );
			fcb->fcbPLen  = size;
		}
		
		fcb->fcbEOF = size;
		
		fast_memcpy( fcb->fcbBfAdr, existing_data.data(), size );
	}
}

OSErr save_app_data( const FCB* fcb )
{
	temp_A4 a4;
	
	plus::string data( fcb->fcbBfAdr, fcb->fcbPLen, plus::delete_never );
	
	int err = try_to_put( appfs_fd, data_path, data );
	
	return err ? ioErr : noErr;
}
