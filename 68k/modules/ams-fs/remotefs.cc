/*
	remotefs.cc
	-----------
*/

#include "remotefs.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <stddef.h>

// plus
#include "plus/var_string.hh"

// ams-common
#include "callouts.hh"
#include "module_A4.hh"

// ams-fs
#include "freemount.hh"


#define STRLEN( s )      (sizeof "" s - 1)
#define STR_LEN( s )  s, (sizeof "" s - 1)

#define GETFINFO  "..namedfork/GetFInfo"


const Byte* remotefs_lookup( VCB* vcb, const Byte* name )
{
	if ( name == NULL  ||  name[ 0 ] == '\0' )
	{
		return NULL;
	}
	
	return name;
}

OSErr remotefs_GetFileInfo( int in, int out, HFileParam* pb, const Byte* name )
{
	temp_A4 a4;
	
	Size len = *name++;
	
	if ( len  &&  *name == ':' )
	{
		++name;
		--len;
	}
	
	char path[ 256 + STRLEN( "/" GETFINFO ) ];
	
	fast_memcpy( path, name, len );
	fast_memcpy( path + len, STR_LEN( "/" GETFINFO ) );
	
	len += STRLEN( "/" GETFINFO );
	
	plus::var_string file_info;
	
	int err = try_to_get( in, out, path, len, file_info );
	
	if ( err < 0 )
	{
		// TODO:  Check for other errors.
		return fnfErr;
	}
	
	const Size size = sizeof (FileParam) - offsetof( FileParam, ioFlAttrib );
	
	if ( file_info.size() != size )
	{
		return ioErr;
	}
	
	if ( pb->ioFDirIndex > 0  &&  pb->ioNamePtr )
	{
		--name;
		
		fast_memcpy( pb->ioNamePtr, name, 1 + name[ 0 ] );
	}
	
	pb->ioFRefNum = 0;
	
	fast_memcpy( &pb->ioFlAttrib, file_info.data(), size );
	
	return noErr;
}
