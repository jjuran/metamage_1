/*	==========
 *	Retrace.cp
 *	==========
 */

#include "ClassicToolbox/Retrace.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( VerticalRetraceManager )
	
	
	static void RegisterVerticalRetraceManagerErrors();
	
	
#pragma force_active on
	
	class VerticalRetraceManagerErrorsRegistration
	{
		public:
			VerticalRetraceManagerErrorsRegistration()  { RegisterVerticalRetraceManagerErrors(); }
	};
	
	static VerticalRetraceManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
	
	
#if CALL_NOT_IN_CARBON
	
	Nucleus::Owned< SlotVBLTask > SlotVInstall( VBLTask& vblTask, short slot )
	{
		ThrowOSStatus( ::SlotVInstall( reinterpret_cast< ::QElemPtr >( &vblTask ), slot ) );
		
		return Nucleus::Owned< SlotVBLTask >::Seize( SlotVBLTask( &vblTask, slot ) );
	}
	
	void SlotVRemove( Nucleus::Owned< SlotVBLTask > slotVBLTask )
	{
		SlotVBLTask released = slotVBLTask.Release();
		
		ThrowOSStatus( ::SlotVRemove( reinterpret_cast< ::QElemPtr >( released.task ),
		                              released.slot ) );
	}
	
	Nucleus::Owned< VBLTaskPtr > VInstall( VBLTask& vblTask )
	{
		ThrowOSStatus( ::VInstall( reinterpret_cast< ::QElemPtr >( &vblTask ) ) );
		
		return Nucleus::Owned< VBLTaskPtr >::Seize( &vblTask );
	}
	
	void VRemove( Nucleus::Owned< VBLTaskPtr > vblTask )
	{
		ThrowOSStatus( ::VRemove( reinterpret_cast< ::QElemPtr >( vblTask.Release() ) ) );
	}
	
#endif  // CALL_NOT_IN_CARBON
	
	void RegisterVerticalRetraceManagerErrors()
	{
		RegisterOSStatus< qErr       >();
		RegisterOSStatus< vTypErr    >();
		RegisterOSStatus< slotNumErr >();
	}
	
}

