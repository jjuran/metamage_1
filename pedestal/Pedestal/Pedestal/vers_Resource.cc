/*
	vers_Resource.cc
	----------------
*/

#include "Pedestal/vers_Resource.hh"

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

#define TRUNCATED  "TRUNCATED 'vers' RESOURCE"


namespace Pedestal
{
	
	bool Get_vers_ShortVersionString( short id, Str255 data )
	{
		if ( Handle h = Get1Resource( 'vers', id ) )
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
				mempcpy( data, "\p" STR_LEN( TRUNCATED ) + 1 );
				return true;
			}
			
			const unsigned char* shortVersion = (unsigned char*) *h + 6;
			
			const UInt16 shortLen = shortVersion[ 0 ];
			
			if ( physical_size < 6 + 1 + shortLen + 1 )
			{
				// Long version length byte overruns resource
				mempcpy( data, "\p" STR_LEN( TRUNCATED ) + 1 );
				return true;
			}
			
			/*
				Validate the entire resource (including the long version)
				before returning the short version string.
			*/
			
			const unsigned char* longVersion = shortVersion + 1 + shortLen;
			
			const UInt16 longLen = longVersion[ 0 ];
			
			if ( physical_size < 6 + 1 + shortLen + 1 + longLen )
			{
				// Long version string data overruns resource
				mempcpy( data, "\p" STR_LEN( TRUNCATED ) + 1 );
				return true;
			}
			
			mempcpy( data, shortVersion, 1 + shortLen );
			
			ReleaseResource( h );
			
			return true;
		}
		
		return false;
	}
	
}
