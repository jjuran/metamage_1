/*	=========
 *	Retrace.h
 *	=========
 */

#ifndef CLASSICTOOLBOX_RETRACE_H
#define CLASSICTOOLBOX_RETRACE_H

// Mac OS
#ifndef __RETRACE__
#include <Retrace.h>
#endif

// Nucleus
#include "Nucleus/ErrorsRegistered.h"

// Nitrogen
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( VerticalRetraceManager );
	
	
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
	
}

namespace Nucleus
{
	
	template <>
	struct Disposer< Nitrogen::VBLTaskPtr > : public  std::unary_function< Nitrogen::VBLTaskPtr, void >,
	                                          private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::VBLTaskPtr vblTaskPtr ) const
		{
			HandleDestructionOSStatus( ::VRemove( reinterpret_cast< ::QElemPtr >( vblTaskPtr ) ) );
		}
	};
	
	template <>
	struct Disposer< Nitrogen::SlotVBLTask > : public  std::unary_function< Nitrogen::SlotVBLTask, void >,
	                                           private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::SlotVBLTask slotVBLTask ) const
		{
			HandleDestructionOSStatus( ::SlotVRemove( reinterpret_cast< ::QElemPtr >( slotVBLTask.task ),
			                                          slotVBLTask.slot ) );
		}
	};
	
}

namespace Nitrogen
{
	
	namespace Private
	{
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		
		inline void InvokeVBLUPP( VBLTaskPtr vblTaskPtr, ::VBLUPP userUPP )
		{
			::InvokeVBLUPP( vblTaskPtr, userUPP );
		}
		
	#else
		
		using ::InvokeVBLUPP;
		
	#endif
	}
	
	struct VBLUPP_Details : Basic_UPP_Details< ::VBLUPP,
	                                           ::VBLProcPtr,
	                                           ::NewVBLUPP,
	                                           ::DisposeVBLUPP,
	                                           Private::InvokeVBLUPP >
	{};
	
	typedef UPP< VBLUPP_Details > VBLUPP;
	
	inline Nucleus::Owned< VBLUPP > NewVBLUPP( ::VBLProcPtr p )
	{
		return NewUPP< VBLUPP >( p );
	}
	
	inline void DisposeVBLUPP( Nucleus::Owned< VBLUPP > )  {}
	
#if !TARGET_CPU_68K || TARGET_RT_MAC_CFM
	
	inline void InvokeVBLUPP( VBLTaskPtr  vblTaskPtr,
	                          VBLUPP      userUPP )
	{
		userUPP( vblTaskPtr );
	}
	
#endif
	
	Nucleus::Owned< SlotVBLTask > SlotVInstall( VBLTask& vblTask, short slot );
	
	void SlotVRemove( Nucleus::Owned< SlotVBLTask > vblTask );
	
	// ...
	
	Nucleus::Owned< VBLTaskPtr > VInstall( VBLTask& vblTask );
	
	void VRemove( Nucleus::Owned< VBLTaskPtr > vblTask );
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	namespace Detail
	{
		
		typedef pascal void (*StackBased_VBLProcPtr)( VBLTaskPtr task );
		
		template < StackBased_VBLProcPtr proc >
		pascal void CallStackBasedVBLProcPtr()
		{
			asm
			{
				MOVE.L   A0, -(SP) ;  // taskPtr
				
				JSR      proc      ;
			}
		}
		
	}
	
	template < Detail::StackBased_VBLProcPtr proc >
	struct VBLProcPtr_Traits
	{
		static VBLProcPtr GetProcPtr()
		{
			return &Detail::CallStackBasedVBLProcPtr< proc >;
		}
	};
	
#else
	
	template < VBLProcPtr proc >
	struct VBLProcPtr_Traits
	{
		static VBLProcPtr GetProcPtr()
		{
			return proc;
		}
	};
	
#endif  // TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
}

#endif

