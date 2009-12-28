/*
	
	Install.cc
	
	Joshua Juran
	
*/

#include "Silver/Install.hh"

// Universal Interfaces
#include <Resources.h>


#pragma exceptions off


namespace Silver
{
	
	inline bool WindowManagerInitialized()
	{
		const UInt8& WWExist = *(UInt8*) 0x08f2;
		
		return WWExist == 0;
	}
	
	
	static OSErr GetAndLockResource( OSType resType, short resID )
	{
		Handle h = Get1Resource( resType, resID );
		
		if ( h == NULL )
		{
			if ( const OSErr err = ResError() )
			{
				return err;
			}
			
			return resNotFound;
		}
		
		DetachResource( h );
		
		if ( const OSErr err = ResError() )
		{
			return err;
		}
		
		HLockHi ( h );
		HNoPurge( h );
		
		return noErr;
	}
	
#if !TARGET_API_MAC_CARBON
	
	static OSErr LoadAndLock()
	{
		THz oldZone = NULL;
		
		if ( !WindowManagerInitialized() )
		{
			oldZone = GetZone();
			
			SetZone( SystemZone() );
		}
		
		const OSErr err = GetAndLockResource( 'INIT', 0 );
		
		if ( oldZone )
		{
			SetZone( oldZone );
		}
		
		return err;
	}
	
	OSErr Install( InstallerProcPtr installer )
	{
		OSErr err = LoadAndLock();
		
		if ( err != noErr )
		{
			return err;
		}
		
		return installer();
	}
	
#endif
	
}

