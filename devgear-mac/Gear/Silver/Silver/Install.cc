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
	
	
	OSErr gError = noErr;
	
	static Handle GetAndLockResource( OSType resType, short resID )
	{
		Handle h = Get1Resource( resType, resID );
		
		if ( (gError = MemError()) != noErr )
		{
			return NULL;
		}
		
		if ( h == NULL )
		{
			gError = resNotFound;
			
			return NULL;
		}
		
		DetachResource( h );
		
		if ( (gError = MemError()) != noErr )
		{
			return NULL;
		}
		
		HLockHi ( h );
		HNoPurge( h );
		
		return h;
	}
	
	static Handle LoadAndLock()
	{
		THz oldZone = NULL;
		
		if ( !WindowManagerInitialized() )
		{
			oldZone = GetZone();
			
			SetZone( SystemZone() );
		}
		
		Handle initCode = GetAndLockResource( 'INIT', 0 );
		
		if ( oldZone )
		{
			SetZone( oldZone );
		}
		
		return initCode;
	}
	
	OSErr Install( InstallerProcPtr installer )
	{
		Handle initCode = LoadAndLock();
		
		if ( initCode == NULL )
		{
			return gError;
		}
		
		return installer();
	}
	
}

