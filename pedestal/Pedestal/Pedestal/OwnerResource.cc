/*
	OwnerResource.cc
	----------------
*/

#include "Pedestal/OwnerResource.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif


#pragma exceptions off


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
	
}
