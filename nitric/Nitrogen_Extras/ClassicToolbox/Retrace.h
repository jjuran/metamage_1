/*	=========
 *	Retrace.h
 *	=========
 */

#ifndef CLASSICTOOLBOX_RETRACE_H
#define CLASSICTOOLBOX_RETRACE_H

// Universal Interfaces
#ifndef __RETRACE__
#include <Retrace.h>
#endif

// Nitrogen
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif


namespace Nitrogen
{
	
	void RegisterVerticalRetraceManagerErrors();
	
#if CALL_NOT_IN_CARBON
	
	using ::VBLTask;
	using ::VBLTaskPtr;
	
	struct SlotVBLTask
	{
		VBLTask*  task;
		short     slot;
		
		SlotVBLTask() : task( NULL ), slot( 0 )  {}
		
		SlotVBLTask( VBLTask*  task,
		             short     slot )
		:
			task( task ),
			slot( slot )
		{}
	};
	
	inline bool operator==( const SlotVBLTask& a, const SlotVBLTask& b )
	{
		return a.task == b.task
		    && a.slot == b.slot;
	}
	
	inline bool operator!=( const SlotVBLTask& a, const SlotVBLTask& b )
	{
		return !( a == b );
	}
	
	template <>
	struct Disposer< VBLTaskPtr > : public  std::unary_function< VBLTaskPtr, void >,
	                                private DefaultDestructionOSStatusPolicy
	{
		void operator()( VBLTaskPtr vblTaskPtr ) const
		{
			HandleDestructionOSStatus( ::VRemove( reinterpret_cast< QElemPtr >( vblTaskPtr ) ) );
		}
	};
	
	template <>
	struct Disposer< SlotVBLTask > : public  std::unary_function< SlotVBLTask, void >,
	                                 private DefaultDestructionOSStatusPolicy
	{
		void operator()( SlotVBLTask slotVBLTask ) const
		{
			HandleDestructionOSStatus( ::SlotVRemove( reinterpret_cast< QElemPtr >( slotVBLTask.task ),
			                                          slotVBLTask.slot ) );
		}
	};
	
	struct VBLUPP_Details : Basic_UPP_Details< ::VBLUPP,
	                                           ::VBLProcPtr,
	                                           ::NewVBLUPP,
	                                           ::DisposeVBLUPP,
	                                           ::InvokeVBLUPP >
	{};
	
	typedef UPP< VBLUPP_Details > VBLUPP;
	
	inline Owned< VBLUPP > NewVBLUPP( ::VBLProcPtr p )
	{
		return NewUPP< VBLUPP >( p );
	}
	
	inline void DisposeVBLUPP( Owned< VBLUPP > )  {}
	
	inline void InvokeVBLUPP( VBLTaskPtr  vblTaskPtr,
	                          VBLUPP      userUPP )
	{
		userUPP( vblTaskPtr );
	}
	
	Owned< SlotVBLTask > SlotVInstall( VBLTask& vblTask, short slot );
	
	void SlotVRemove( Owned< SlotVBLTask > vblTask );
	
	// ...
	
	Owned< VBLTaskPtr > VInstall( VBLTask& vblTask );
	
	void VRemove( Owned< VBLTaskPtr > vblTask );
	
#endif  // CALL_NOT_IN_CARBON
	
}

#endif

