/*
	find_MPW_dir.cc
	-----------------
*/

#include "one_path/find_MPW_dir.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// mac-file-utils
#ifdef __RELIX__
#include "mac_file/desktop.hh"
#endif

// gear
#include "gear/inscribe_decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"


#define STRLEN(s)  (sizeof "" s - 1)


namespace p7 = poseven;


const int max_path_len = sizeof "/.hfs/12345/1234567890";

static char path_buffer[ max_path_len ] = "/.hfs/";

plus::string find_MPW_dir()
{
	if ( const char* mpw_dir = getenv( "MPW_DIR" ) )
	{
		return mpw_dir;
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
		throw p7::errno_t( ENOENT );
	}
	
	char* p = path_buffer + STRLEN( "/.hfs/" );
	
	p = gear::inscribe_unsigned_decimal_r( -file.vRefNum, p );
	
	*p++ = '/';
	
	p = gear::inscribe_unsigned_decimal_r( file.parID, p );
	
	*p = '\0';
	
	ssize_t size = _realpath( path_buffer, NULL, 0 );
	
	if ( size < 0 )
	{
		if ( size == -1 )
		{
			p7::throw_errno( errno );
		}
		
		size = ~size;
		
		plus::string result;
		
		char* p = result.reset( size );
		
		size = _realpath( path_buffer, p, size );
		
		return result;
	}
	
#endif  // #ifdef __RELIX__
	
	throw p7::errno_t( ENOSYS );
}
