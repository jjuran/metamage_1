/*
	OwnerResource.cc
	----------------
*/

#include "Pedestal/OwnerResource.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C
#include <string.h>


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
	
	bool GetOwnerResourceData( OSType creator, Str255 data )
	{
		if ( Handle h = GetResource( creator, 0 ) )
		{
			if ( const UInt32 physical_size = GetHandleSize( h ) )
			{
				const unsigned char* text = (unsigned char*) *h;
				
				const UInt16 logical_size = 1 + text[ 0 ];
				
				if ( logical_size <= physical_size )
				{
					memcpy( data, text, logical_size );
				}
				else
				{
					memcpy( data, "\p" STR_LEN( TRUNCATED ) + 1 );
				}
			}
			else
			{
				memcpy( data, "\p" STR_LEN( EMPTY ) + 1 );
			}
			
			ReleaseResource( h );
			
			return true;
		}
		
		return false;
	}
	
}
