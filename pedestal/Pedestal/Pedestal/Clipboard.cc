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
	
	
	static const bool gSyncTEScrapOnEdit = TARGET_API_MAC_CARBON;
	
	
	static bool InFront()
	{
		return N::SameProcess( N::GetFrontProcess(), N::CurrentProcess() );
	}
	
	Clipboard::Clipboard()
	{
		if ( !gSyncTEScrapOnEdit  &&  InFront() )
		{
			Resume();
		}
	}
	
	Clipboard::~Clipboard()
	{
		if ( !gSyncTEScrapOnEdit  &&  InFront() )
		{
			Suspend();
		}
	}
	
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
	
	void Clipboard::Suspend()
	{
		if ( !gSyncTEScrapOnEdit )
		{
			FlushScrap();
		}
	}
	
	void Clipboard::Resume()
	{
		if ( !gSyncTEScrapOnEdit )
		{
			OSErr err = ::TEFromScrap();
			
			// We'll get an error if there's nothing in the clipboard,
			// but this is perfectly reasonable at startup.
			
			if ( err != noTypeErr )
			{
				N::ThrowOSStatus( err );
			}
		}
	}
	
	static void PreTECopy()
	{
		ClearCarbonScrap();
	}
	
	static void PostTECopy()
	{
		if ( gSyncTEScrapOnEdit )
		{
			// Flush the TE scrap immediately
			FlushScrap();
		}
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
		if ( gSyncTEScrapOnEdit )
		{
			// Update the TE scrap just-in-time
			N::TEFromScrap();
		}
		
		N::TEPaste( hTE );
	}
	
}

