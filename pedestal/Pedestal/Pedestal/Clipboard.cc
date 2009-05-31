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
#include "ClassicToolbox/Scrap.h"
#endif


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static void ClearCarbonScrap()
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
	
	static void PreTECopy()
	{
		ClearCarbonScrap();
	}
	
	static void PostTECopy()
	{
		// Flush the TE scrap immediately
		FlushScrap();
	}
	
	void Clipboard::TECut( TEHandle hTE )
	{
		PreTECopy();
		
		N::TECut( hTE );
		
		PostTECopy();
	}
	
	void Clipboard::TECopy( TEHandle hTE )
	{
		PreTECopy();
		
		N::TECopy( hTE );
		
		PostTECopy();
	}
	
	void Clipboard::TEPaste( TEHandle hTE )
	{
		// Update the TE scrap just-in-time
		N::TEFromScrap();
		
		N::TEPaste( hTE );
	}
	
}

