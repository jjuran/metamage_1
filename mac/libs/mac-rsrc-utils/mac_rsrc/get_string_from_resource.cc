/*
	get_string_from_resource.cc
	---------------------------
*/

#include "mac_rsrc/get_string_from_resource.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// more-libc
#include "more/string.h"

// mac-glue-utils
#include "mac_glue/Memory.hh"


#pragma exceptions off


namespace mac  {
namespace rsrc {

signed char get_string_from_resource( unsigned char*  result,
                                      unsigned long   type,
                                      short           id )
{
	if ( Handle h = GetResource( type, id ) )
	{
		const Size physical_size = mac::glue::GetHandleSize_raw( h );
		
		if ( physical_size > 0 )
		{
			const Byte* text = (Byte*) *h;
			
			const UInt16 logical_size = 1 + text[ 0 ];
			
			if ( logical_size <= physical_size )
			{
				mempcpy( result, text, logical_size );
				
				return true;
			}
		}
	}
	
	result[ 0 ] = 0;
	
	return false;
}

}
}
