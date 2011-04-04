/*	============
 *	Clipboard.cc
 *	============
 */

#include "Pedestal/Clipboard.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"

#include "Nitrogen/Scrap.hh"
#include "Nitrogen/TextEdit.hh"


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
		catch ( const Mac::OSStatus& err )
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

