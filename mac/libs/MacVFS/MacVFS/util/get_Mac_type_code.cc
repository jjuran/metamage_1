/*
	get_Mac_type_code.cc
	--------------------
*/

#include "MacVFS/util/get_Mac_type_code.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// MacVFS
#include "MacVFS/util/FSSpec_from_node.hh"


namespace vfs
{
	
	unsigned long get_Mac_type_code( const node& file )
	{
		FSSpec fsspec = FSSpec_from_node( file );
		
		FInfo info;
		
		OSErr err = ::FSpGetFInfo( &fsspec, &info );
		
		if ( err != noErr )
		{
			return 0;
		}
		
		return info.fdType;
	}
	
}
