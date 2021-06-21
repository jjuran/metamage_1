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


#pragma exceptions off


namespace mac  {
namespace rsrc {
	
	bool get_string_from_resource( unsigned char*  result,
	                               unsigned long   type,
	                               short           id )
	{
		if ( Handle h = GetResource( type, id ) )
		{
			if ( Size size = GetHandleSize( h ) )
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
