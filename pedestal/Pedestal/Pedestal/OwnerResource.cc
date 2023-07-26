/*
	OwnerResource.cc
	----------------
*/

#include "Pedestal/OwnerResource.hh"

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

#define TRUNCATED  "TRUNCATED 'STR ' RESOURCE"
#define EMPTY      "EMPTY 'STR ' RESOURCE"


namespace Pedestal
{
	
	OSType GetCreatorFromBNDL()
	{
		if ( Handle bndl = GetResource( 'BNDL', 128 ) )
		{
			const OSType creator = *(OSType*) *bndl;
			
			ReleaseResource( bndl );
			
			return creator;
		}
		
		return 0;
	}
	
	bool GetOwnerResourceName( OSType creator, Str255 name )
	{
		if ( Handle h = GetResource( creator, 0 ) )
		{
			GetResInfo( h, NULL, NULL, name );
			
			ReleaseResource( h );
			
			return true;
		}
		
		return false;
	}
	
	bool GetOwnerResourceData( OSType creator, Str255 data )
	{
		if ( Handle h = GetResource( creator, 0 ) )
		{
			const Size physical_size = mac::glue::GetHandleSize_raw( h );
			
			if ( physical_size > 0 )
			{
				const unsigned char* text = (unsigned char*) *h;
				
				const UInt16 logical_size = 1 + text[ 0 ];
				
				if ( logical_size <= physical_size )
				{
					mempcpy( data, text, logical_size );
				}
				else
				{
					mempcpy( data, "\p" STR_LEN( TRUNCATED ) + 1 );
				}
			}
			else
			{
				mempcpy( data, "\p" STR_LEN( EMPTY ) + 1 );
			}
			
			ReleaseResource( h );
			
			return true;
		}
		
		return false;
	}
	
}
