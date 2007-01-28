/*	============
 *	Clipboard.cc
 *	============
 */

#include "Pedestal/Clipboard.hh"

// Nitrogen
#include "Nitrogen/Scrap.h"
#include "Nitrogen/TextEdit.h"

// Nitrogen Extras / ClassicToolbox
#if !TARGET_API_MAC_CARBON
#include "ClassicToolbox/Scrap.h"
#endif


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	Clipboard::Clipboard()
	{
		Resume();
	}
	
	Clipboard::~Clipboard()
	{
		Suspend();
	}
	
	void Clipboard::Suspend()
	{
	#if !TARGET_API_MAC_CARBON
		
		N::ZeroScrap();
		N::TEToScrap();
		
	#endif
	}
	
	void Clipboard::Resume()
	{
	#if !TARGET_API_MAC_CARBON
		
		try
		{
			N::TEFromScrap();
		}
		catch ( const N::OSStatus& err )
		{
			// If there's nothing in the clipboard, which is perfectly reasonable, 
			// we'll get an error, so ignore it.
		}
		
	#endif
	}
	
	static void PreTECopy()
	{
	#if TARGET_API_MAC_CARBON
		
		N::ClearCurrentScrap();
		
	#endif
	}
	
	static void PostTECopy()
	{
	#if TARGET_API_MAC_CARBON
		
		try
		{
			N::TEToScrap();
		}
		catch ( const N::OSStatus& )
		{
			N::ClearCurrentScrap();
		}
		
	#endif
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
		if ( TARGET_API_MAC_CARBON )
		{
			N::TEFromScrap();
		}
		
		N::TEPaste( hTE );
	}
	
}

