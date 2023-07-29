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


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define STR_RESOURCE  "'STR ' RESOURCE (or equivalent type)"

#define TRUNCATED_STR  "TRUNCATED " STR_RESOURCE
#define EMPTY_STR      "EMPTY " STR_RESOURCE

#define TRUNCATED_vers  "TRUNCATED 'vers' RESOURCE"


namespace mac  {
namespace rsrc {

signed char get_string_from_handle( unsigned char*  result,
                                    Handle          h )
{
	if ( h != NULL )
	{
		/*
			We're not going to release the resource -- we have
			no way to know if anyone else might be using it.
		*/
		
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
			else
			{
				mempcpy( result, "\p" STR_LEN( TRUNCATED_STR ) + 1 );
			}
		}
		else
		{
			mempcpy( result, "\p" STR_LEN( EMPTY_STR ) + 1 );
		}
		
		/*
			Indicate errors with a special value that evaluates as true,
			so that by default, the error message will be visible.
		*/
		
		return -true;
	}
	
	result[ 0 ] = 0;
	
	return false;
}

signed char get_string_from_resource( unsigned char*  result,
                                      unsigned long   type,
                                      short           id )
{
	return get_string_from_handle( result, GetResource( type, id ) );
}

signed char get_vers_ShortVersionString( unsigned char* result, short id )
{
	if ( Handle h = GetResource( 'vers', id ) )
	{
		const Size physical_size = mac::glue::GetHandleSize_raw( h );
		
		/*
			Contents: 4-byte version, 2-byte country code, 2 Pascal strings
			
			Minimum size is 8 bytes (with two empty strings).
			
			Return true on invalid resource data, so it gets exposed.
		*/
		
		if ( physical_size < 4 + 2 + 1 + 1 )
		{
			// Less than minimum length, or an error somehow occurred
			mempcpy( result, "\p" STR_LEN( TRUNCATED_vers ) + 1 );
			return -true;
		}
		
		const Byte* shortVersion = (Byte*) *h + 6;
		
		const UInt16 shortLen = shortVersion[ 0 ];
		
		if ( physical_size < 6 + 1 + shortLen + 1 )
		{
			// Long version length byte overruns resource
			mempcpy( result, "\p" STR_LEN( TRUNCATED_vers ) + 1 );
			return -true;
		}
		
		/*
			Validate the entire resource (including the long version)
			before returning the short version string.
		*/
		
		const Byte* longVersion = shortVersion + 1 + shortLen;
		
		const UInt16 longLen = longVersion[ 0 ];
		
		if ( physical_size < 6 + 1 + shortLen + 1 + longLen )
		{
			// Long version string data overruns resource
			mempcpy( result, "\p" STR_LEN( TRUNCATED_vers ) + 1 );
			return -true;
		}
		
		mempcpy( result, shortVersion, 1 + shortLen );
		
		return true;
	}
	
	return false;
}

}
}
