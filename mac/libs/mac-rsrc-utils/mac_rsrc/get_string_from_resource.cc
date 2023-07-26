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
			const Size size = mac::glue::GetHandleSize_raw( h );
			
			if ( size > 0 )
			{
				unsigned char len = **h;
				
				if ( size > len )
				{
					mempcpy( result, *h, len + 1 );
					
					return true;
				}
			}
		}
		
		result[ 0 ] = 0;
		
		return false;
	}
	
}
}
