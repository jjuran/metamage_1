/*
	find_MPW_dir.cc
	-----------------
*/

#include "one_path/find_MPW_dir.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdlib.h>

// mac-file-utils
#ifdef __RELIX__
#include "mac_file/desktop.hh"
#endif

// gear
#include "gear/inscribe_decimal.hh"

// vxo
#include "vxo/errno.hh"

// vxo-string
#include "vxo-string/string.hh"

// vxo-posix
#include "vxo-posix/realpath.hh"


#pragma exceptions off


#define STRLEN(s)  (sizeof "" s - 1)


const int max_path_len = sizeof "/.hfs/12345/1234567890";

static char path_buffer[ max_path_len ] = "/.hfs/";

vxo::Box find_MPW_dir()

{
	if ( const char* mpw_dir = getenv( "MPW_DIR" ) )
	{
		return vxo::StaticString( mpw_dir );
	}
	
#ifdef __RELIX__
	
	using mac::file::get_desktop_APPL;
	using mac::file::get_desktop_APPL_on_RAM_disk;
	
	OSErr err;
	FSSpec file;
	
	err = get_desktop_APPL_on_RAM_disk( file, 'MPSX' );
	
	if ( err )
	{
		err = get_desktop_APPL( file, 'MPSX' );
	}
	
	if ( err )
	{
		return vxo::Errno( ENOENT );
	}
	
	char* p = path_buffer + STRLEN( "/.hfs/" );
	
	p = gear::inscribe_unsigned_decimal_r( -file.vRefNum, p );
	
	*p++ = '/';
	
	p = gear::inscribe_unsigned_decimal_r( file.parID, p );
	
	*p = '\0';
	
	return vxo::realpath( path_buffer );
	
#endif  // #ifdef __RELIX__
	
	return vxo::Errno( ENOSYS );
}
