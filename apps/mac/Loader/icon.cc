/*
	icon.cc
	-------
*/

#include "icon.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif


static
long get_local_id_for_type( OSType file_type )
{
	/*
		This function leaks 'FREF' resources in the context of the containing
		resource file being open, but soon afterward the resource file is
		closed, releasing all undetached resources gotten from it.
	*/
	
	short i = 1;
	
	while ( Handle fref = Get1IndResource( 'FREF', i ) )
	{
		const UInt32* longs = (const UInt32*) *fref;
		
		const OSType type = *longs++;
		
		if ( type == file_type )
		{
			const UInt16* words = (const UInt16*) longs;
			
			return *words;
		}
		
		++i;
	}
	
	return -1;
}

static
long get_icon_res_id_for_local_id( short bndl_local_id )
{
	/*
		This function leaks a 'BNDL' resource in the context of the containing
		resource file being open, but soon afterward the resource file is
		closed, releasing all undetached resources gotten from it.
	*/
	
	if ( Handle bndl = Get1IndResource( 'BNDL', 1 ) )
	{
		const UInt32* p = (const UInt32*) *bndl;
		
		OSType owner = *p++;
		
		long n_types_1 = *p++;  // count - 1
		
		do
		{
			ResType type = *p++;
			
			const UInt16* q = (const UInt16*) p;
			
			short n_rsrcs_1 = *q++;  // count - 1 
			
			if ( type != 'ICN#' )
			{
				p = (const UInt32*) q + (n_rsrcs_1 + 1);
				continue;
			}
			
			do
			{
				UInt16 local_id = *q++;
				UInt16 res_id   = *q++;
				
				if ( local_id == bndl_local_id )
				{
					return res_id;
				}
			}
			while ( n_rsrcs_1-- > 0 );
		}
		while ( n_types_1-- > 0 );
	}
	
	return -1;
}

Handle get_icon( Str255 name )
{
	Handle icon = NULL;
	
	short refnum = OpenResFile( name );
	
	if ( refnum > 0 )
	{
		const long local_id = get_local_id_for_type( 'APPL' );
		
		if ( local_id >= 0 )
		{
			const long icon_id = get_icon_res_id_for_local_id( local_id );
			
			if ( icon_id >= 0 )
			{
				icon = Get1Resource( 'ICN#', icon_id );
				
				if ( icon )
				{
					DetachResource( icon );
				}
			}
		}
		
		CloseResFile( refnum );
	}
	
	return icon;
}
