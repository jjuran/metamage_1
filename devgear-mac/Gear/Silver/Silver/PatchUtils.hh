/*
	
	PatchUtils.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCHUTILS_HH
#define SILVER_PATCHUTILS_HH

// Universal Interfaces
#include <Resources.h>

// Needed for globals
#include <A4Stuff.h>


namespace Silver
{

	class MyA4
	{
		private:
			long oldA4;
		
		public:
			MyA4() : oldA4( ::SetCurrentA4() )  {}
			
			~MyA4()  { ::SetA4( oldA4 ); }
	};
	
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
	
	
	static ProcPtr ApplyTrapPatch_( short trap, ProcPtr patchPtr )
	{
		if ( patchPtr == NULL )
		{
			return NULL;
		}
		
		TrapType trapType = (trap & 0x0800) ? ToolTrap : OSTrap;
		
		ProcPtr trapPtr = ::NGetTrapAddress( trap, trapType );
		::NSetTrapAddress( patchPtr, trap, trapType );
		
		return trapPtr;
	}
	
	inline void RemoveTrapPatch_( short trap, ProcPtr patchPtr )
	{
		ApplyTrapPatch_( trap, patchPtr );
	}
	
	template < class PatchedProcPtr >
	PatchedProcPtr ApplyTrapPatch( short trap, PatchedProcPtr patchPtr )
	{
		ProcPtr trapPtr = ApplyTrapPatch_( trap,
		                                   reinterpret_cast< ProcPtr >( patchPtr ) );
		
		return reinterpret_cast< PatchedProcPtr >( trapPtr );
	}
	
	template < class PatchedProcPtr >
	void RemoveTrapPatch( short trap, PatchedProcPtr patchPtr )
	{
		RemoveTrapPatch_( trap, reinterpret_cast< ProcPtr >( patchPtr ) );
	}
	
	
	static Handle GetAndLockResource( OSType resType, short resID )
	{
		Handle h = Get1Resource( resType, resID );
		if ( h == NULL )  return NULL;
		
		DetachResource( h );
		if ( ResError() != noErr )  return NULL;
		
		HLockHi( h );
		HNoPurge( h );
		
		return h;
	}
	
	static bool LoadAndLock()
	{
		TempZone tempZone = SystemZone();
		
		Handle initCode = GetAndLockResource( 'INIT', 0 );
		
		return initCode != NULL;
	}
	
}
#endif

