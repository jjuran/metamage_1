/*
	
	Install.cc
	
	Joshua Juran
	
*/

#include "Silver/Install.hh"

// Universal Interfaces
#include <Resources.h>

// Silver
#include "Silver/CurrentA4.hh"


#pragma exceptions off


namespace Silver
{

	class TempZone
	{
		private:
			THz oldZone;
		
		public:
			TempZone( THz zone ) : oldZone( ::GetZone() )
			{
				::SetZone( zone );
			}
			
			~TempZone()  { ::SetZone( oldZone ); }
	};
	
	
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
	
	static bool LoadAndLock()
	{
		TempZone tempZone = SystemZone();
		
		Handle initCode = GetAndLockResource( 'INIT', 0 );
		
		return initCode != NULL;
	}
	
	OSErr Install( InstallerProcPtr installer )
	{
		CurrentA4 a4;
		
		bool locked = LoadAndLock();
		
		if ( !locked )
		{
			return gError;
		}
		
		return installer();
	}
	
}

