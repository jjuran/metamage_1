// Nitrogen/OSUtils.hh
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_OSUTILS_HH
#define NITROGEN_OSUTILS_HH

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_MIXEDMODE_HH
#include "Nitrogen/MixedMode.hh"
#endif
#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif
#ifndef NITROGEN_DATETIMEUTILS_HH
#include "Nitrogen/DateTimeUtils.hh"
#endif
#ifndef NITROGEN_CFSTRING_HH
#include "Nitrogen/CFString.hh"
#endif
#ifndef NITROGEN_UPP_HH
#include "Nitrogen/UPP.hh"
#endif


#ifdef GetMMUMode
#undef GetMMUMode

inline pascal SInt8 GetMMUMode()
{
	return true32b;
}

#endif


#ifdef SwapMMUMode
#undef SwapMMUMode

inline pascal void SwapMMUMode( SInt8* mode )
{
	*mode = true32b;
}

#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( DeferredTaskManager );
	
	// Serial port usage
	
	enum MMUMode
	{
		false32b = ::false32b,
		true32b  = ::true32b,
		
		kMMUMode_Max = nucleus::enumeration_traits< SInt8 >::max
	};
	
	// ...
	
	typedef pascal void (*DeferredTaskProcPtr)( long dtParam );
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	struct DeferredTaskUPP_Details
	{
		typedef ::DeferredTaskUPP UPPType;
		
		// This is the stack-based function signature
		typedef DeferredTaskProcPtr ProcPtr;
		
		template < ProcPtr procPtr >
		static pascal void Glue()
		{
			Call_With_A0_Glue< ProcPtr, procPtr >();
		}
	};
	
	typedef GlueUPP< DeferredTaskUPP_Details > DeferredTaskUPP;
	
#else
	
	struct DeferredTaskUPP_Details : Basic_UPP_Details< ::DeferredTaskUPP,
	                                                    ::DeferredTaskProcPtr,
	                                                    ::NewDeferredTaskUPP,
	                                                    ::DisposeDeferredTaskUPP,
	                                                    ::InvokeDeferredTaskUPP >
	{
	};
	
	typedef UPP< DeferredTaskUPP_Details > DeferredTaskUPP;
	
#endif
	
	inline nucleus::owned< DeferredTaskUPP > NewDeferredTaskUPP( DeferredTaskProcPtr p )
	{
		return NewUPP< DeferredTaskUPP >( p );
	}
	
	inline void DisposeDeferredTaskUPP( nucleus::owned< DeferredTaskUPP > )  {}
	
	inline void InvokeDeferredTaskUPP( long             dtParam,
	                                   DeferredTaskUPP  userUPP )
	{
		::InvokeDeferredTaskUPP( dtParam, userUPP );
	}
	
	using ::DeferredTask;
	using ::DeferredTaskPtr;
	
	using ::MachineLocation;
	
	inline bool IsMetric()  { return ::IsMetric(); }
	
	using ::GetSysPPtr;
	
	void DTInstall( DeferredTask& dtTaskPtr );
	
	inline MMUMode GetMMUMode()  { return MMUMode( ::GetMMUMode() ); }
	
	// SwapMMUMode
	
	inline UInt32 Delay( UInt32 ticks )
	{
		UInt32 finalTicks;
		::Delay( ticks, &finalTicks );
		
		return finalTicks;
	}
	
	// WriteParam
	// Enqueue
	// Dequeue
	// SetCurrentA5
	// SetA5
	// InitUtil
	
#if !TARGET_CPU_68K
	
	using ::MakeDataExecutable;
	
#endif
	
	inline void ReadLocation ( MachineLocation      & loc )  { ::ReadLocation ( &loc ); }
	inline void WriteLocation( MachineLocation const& loc )  { ::WriteLocation( &loc ); }
	
	inline MachineLocation ReadLocation()
	{
		MachineLocation result;
		
		ReadLocation( result );
		
		return result;
	}
	
	using ::TickCount;
	
#if	TARGET_RT_MAC_MACHO
/*	
	There's a long note in technical Q & A #1078
		http://developer.apple.com/qa/qa2001/qa1078.html
	about how to make CSCopyMachineName and CSCopyUserName work in all environments (OS 9, CFM, etc)
	I'm going the easy way here, and making it work for Mach-O.
*/

	class CSCopyUserName_Failed {};
	inline nucleus::owned<CFStringRef> CSCopyUserName ( Boolean useShortName ) {
		CFStringRef result = ::CSCopyUserName ( useShortName );
		if ( NULL == result ) throw CSCopyUserName_Failed();
		return nucleus::owned<CFStringRef>::seize ( result );
		}

	class CSCopyMachineName_Failed {};
	inline nucleus::owned<CFStringRef> CSCopyMachineName (void) {
		CFStringRef result = ::CSCopyMachineName();
		if ( NULL == result ) throw CSCopyMachineName_Failed();
		return nucleus::owned<CFStringRef>::seize ( result );
		}
#endif

}

#endif

