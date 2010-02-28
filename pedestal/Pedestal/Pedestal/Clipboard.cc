/*	============
 *	Clipboard.cc
 *	============
 */

#include "Pedestal/Clipboard.hh"

// Nitrogen
#include "Nitrogen/Scrap.h"
#include "Nitrogen/TextEdit.h"

// ClassicToolbox
#if !TARGET_API_MAC_CARBON
#include "ClassicToolbox/Scrap.hh"
#endif


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static inline void ClearCarbonScrap()
	{
		if ( TARGET_API_MAC_CARBON )
		{
			N::ClearCurrentScrap();
		}
	}
	
	static void FlushScrap()
	{
		try
		{
		#if !TARGET_API_MAC_CARBON
			
			N::ZeroScrap();
			
		#endif
			
			N::TEToScrap();
		}
		catch ( const N::OSStatus& err )
		{
			ClearCarbonScrap();
		}
	}
	
	void Clipboard::TECut( TEHandle hTE )
	{
		ClearCarbonScrap();
		
		N::TECut( hTE );
		
		FlushScrap();
	}
	
	void Clipboard::TECopy( TEHandle hTE )
	{
		ClearCarbonScrap();
		
		N::TECopy( hTE );
		
		FlushScrap();
	}
	
	void Clipboard::TEPaste( TEHandle hTE )
	{
		// Update the TE scrap just-in-time
		N::TEFromScrap();
		
		N::TEPaste( hTE );
	}
	
}

