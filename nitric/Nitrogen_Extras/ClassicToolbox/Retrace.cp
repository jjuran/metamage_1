/*	==========
 *	Retrace.cp
 *	==========
 */

#ifndef __MACH__
#ifndef CLASSICTOOLBOX_RETRACE_H
#include "ClassicToolbox/Retrace.h"
#endif
#endif


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	Owned< SlotVBLTask > SlotVInstall( VBLTask& vblTask, short slot )
	{
		ThrowOSStatus( ::SlotVInstall( reinterpret_cast< QElemPtr >( &vblTask ), slot ) );
		
		return Owned< SlotVBLTask >::Seize( SlotVBLTask( &vblTask, slot ) );
	}
	
	void SlotVRemove( Owned< SlotVBLTask > slotVBLTask )
	{
		SlotVBLTask released = slotVBLTask.Release();
		
		ThrowOSStatus( ::SlotVRemove( reinterpret_cast< QElemPtr >( released.task ),
		                              released.slot ) );
	}
	
	Owned< VBLTaskPtr > VInstall( VBLTask& vblTask )
	{
		ThrowOSStatus( ::VInstall( reinterpret_cast< QElemPtr >( &vblTask ) ) );
		
		return Owned< VBLTaskPtr >::Seize( &vblTask );
	}
	
	void VRemove( Owned< VBLTaskPtr > vblTask )
	{
		ThrowOSStatus( ::VRemove( reinterpret_cast< QElemPtr >( vblTask.Release() ) ) );
	}
	
#endif  // CALL_NOT_IN_CARBON
	
	void RegisterVerticalRetraceManagerErrors()
	{
		RegisterOSStatus< qErr       >();
		RegisterOSStatus< vTypErr    >();
		RegisterOSStatus< slotNumErr >();
	}
	
}

