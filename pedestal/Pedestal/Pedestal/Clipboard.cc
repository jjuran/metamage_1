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
	
	
	static bool InFront()
	{
		return N::SameProcess( N::GetFrontProcess(), N::CurrentProcess() );
	}
	
	Clipboard::Clipboard()
	{
		if ( !TARGET_API_MAC_CARBON  &&  InFront() )
		{
			Resume();
		}
	}
	
	Clipboard::~Clipboard()
	{
		if ( !TARGET_API_MAC_CARBON  &&  InFront() )
		{
			Suspend();
		}
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
		
		OSErr err = ::TEFromScrap();
		
		// We'll get an error if there's nothing in the clipboard,
		// but this is perfectly reasonable at startup.
		
		if ( err != noTypeErr )
		{
			N::ThrowOSStatus( err );
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

