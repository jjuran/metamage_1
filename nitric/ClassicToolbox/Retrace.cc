/*	==========
 *	Retrace.cc
 *	==========
 */

#include "ClassicToolbox/Retrace.hh"

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( VerticalRetraceManager )
	
	
	static void RegisterVerticalRetraceManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class VerticalRetraceManagerErrorsRegistration
	{
		public:
			VerticalRetraceManagerErrorsRegistration()  { RegisterVerticalRetraceManagerErrors(); }
	};
	
	static VerticalRetraceManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	nucleus::owned< SlotVBLTask > SlotVInstall( VBLTask& vblTask, short slot )
	{
		ThrowOSStatus( ::SlotVInstall( reinterpret_cast< ::QElemPtr >( &vblTask ), slot ) );
		
		return nucleus::owned< SlotVBLTask >::seize( SlotVBLTask( &vblTask, slot ) );
	}
	
	void SlotVRemove( nucleus::owned< SlotVBLTask > slotVBLTask )
	{
		SlotVBLTask released = slotVBLTask.release();
		
		ThrowOSStatus( ::SlotVRemove( reinterpret_cast< ::QElemPtr >( released.task ),
		                              released.slot ) );
	}
	
	nucleus::owned< VBLTaskPtr > VInstall( VBLTask& vblTask )
	{
		ThrowOSStatus( ::VInstall( reinterpret_cast< ::QElemPtr >( &vblTask ) ) );
		
		return nucleus::owned< VBLTaskPtr >::seize( &vblTask );
	}
	
	void VRemove( nucleus::owned< VBLTaskPtr > vblTask )
	{
		ThrowOSStatus( ::VRemove( reinterpret_cast< ::QElemPtr >( vblTask.release() ) ) );
	}
	
	void RegisterVerticalRetraceManagerErrors()
	{
		RegisterOSStatus< qErr       >();
		RegisterOSStatus< vTypErr    >();
		RegisterOSStatus< slotNumErr >();
	}
	
}

