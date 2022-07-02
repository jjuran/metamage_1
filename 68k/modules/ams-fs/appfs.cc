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
		
		if ( size > fcb->fcbEOF )
		{
			fcb->fcbEOF = size;
		}
		
		fast_memcpy( fcb->fcbBfAdr, existing_data.data(), size );
	}
}

OSErr save_app_data( const FCB* fcb, long length )
{
	temp_A4 a4;
	
	plus::string data( fcb->fcbBfAdr, length, vxo::delete_never );
	
	/*
		The goal here is to preserve any data written to the application's
		data fork, without copying any unwritten portions.  The ur-example
		is Lode Runner, which saves high scores into the first 262 bytes
		of its 34 KiB data fork.
		
		If we're writing less than the length of the file, then first try
		a Freemount write operation instead of a put, to ensure that in the
		event of a selective write, we don't clobber subsequent data.  The
		issue is not read-only application data (which never gets written
		at all), but writing data in chunks -- using a put to write a small
		chunk would clobber anything following that data in the file.
		
		The write will fail if the file doesn't exist yet (which will
		happen if this is our first time saving app data).  In case it
		is, try a put (which will create the file if necessary).  If we're
		writing the entire file anyway, then there's nothing following to be
		clobbered and we can just use a put operation in the first place.
	*/
	
	int err = -1;
	
	if ( length < fcb->fcbPLen )
	{
		err = try_to_write( appfs_fd, data_path, data, 0 );
	}
	
	if ( err )
	{
		err = try_to_put( appfs_fd, data_path, data );
	}
	
	return err ? ioErr : noErr;
}
