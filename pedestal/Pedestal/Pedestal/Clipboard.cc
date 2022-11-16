/*	============
 *	Clipboard.cc
 *	============
 */

#include "Pedestal/Clipboard.hh"

// Mac OS
#ifndef __SCRAP__
#include <Scrap.h>
#endif


#pragma exceptions off


namespace Pedestal
{
	
	static inline void ClearCarbonScrap()
	{
		if ( TARGET_API_MAC_CARBON )
		{
			ClearCurrentScrap();
		}
	}
	
	static void FlushScrap()
	{
	#if !TARGET_API_MAC_CARBON
		
		ZeroScrap();
		
	#endif
		
		OSErr err = TEToScrap();
		
		if ( err != noErr )
		{
			ClearCarbonScrap();
		}
	}
	
	void Clipboard::TECut( TEHandle hTE )
	{
		ClearCarbonScrap();
		
		TECut( hTE );
		
		FlushScrap();
	}
	
	void Clipboard::TECopy( TEHandle hTE )
	{
		ClearCarbonScrap();
		
		TECopy( hTE );
		
		FlushScrap();
	}
	
	void Clipboard::TEPaste( TEHandle hTE )
	{
		// Update the TE scrap just-in-time
		OSErr err = TEFromScrap();
		
		if ( err == noErr )
		{
			TEPaste( hTE );
		}
	}
	
}
