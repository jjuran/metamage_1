/*
	open_data_fork.cc
	-----------------
*/

#include "mac_file/open_data_fork.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifdef __MACOS__
#ifndef __FILES__
#include <Files.h>
#endif
#endif

// more-libc
#include "more/string.h"

// mac-sys-utils
#include "mac_sys/has/FSSpec_calls.hh"


namespace mac  {
namespace file {

FSIORefNum open_data_fork( short                 vRefNum,
                           long                  dirID,
                           const unsigned char*  name,
                           signed char           perm )
{
	OSErr err;
	FSIORefNum result;
	
#if ! __LP64__
	
	Str255 path;
	
	if ( name[ 1 ] == '.' )
	{
		Byte len = name[ 0 ];
		
		Byte* p = path;
		
		*p++ = len + 1;
		*p++ = ':';
		
		mempcpy( p, name + 1, len );
		
		name = path;
	}
	
	err = HOpen( vRefNum, dirID, name, perm, &result );
	
	return err ? err : result;
	
#endif
	
	return 0;
}

FSIORefNum open_data_fork( const FSSpec& file, signed char perm )
{
	OSErr err;
	FSIORefNum result;
	
#if ! __LP64__
	
	if ( mac::sys::has_FSSpec_calls() )
	{
		err = FSpOpenDF( &file, perm, &result );
	}
	else
	{
		return open_data_fork( file.vRefNum, file.parID, file.name, perm );
	}
	
	return err ? err : result;
	
#endif
	
	return 0;
}

FSIORefNum open_data_fork( const FSRef& file, signed char perm )
{
	OSErr err;
	FSIORefNum result;
	
	err = FSOpenFork( &file, 0, NULL, perm, &result );
	
	return err ? err : result;
}

}
}
