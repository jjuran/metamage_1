/*	=========
 *	OSUtils.h
 *	=========
 */

#ifndef NITROGEN_OSUTILS_H
#define NITROGEN_OSUTILS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __OSUTILS__
#include FRAMEWORK_HEADER(CarbonCore,OSUtils.h)
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_MIXEDMODE_H
#include "Nitrogen/MixedMode.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_DATETIMEUTILS_H
#include "Nitrogen/DateTimeUtils.h"
#endif
#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif


namespace Nitrogen
{
	
	void RegisterDeferredTaskManagerErrors();
	
	// ...
	
	struct DeferredTaskUPP_Details : Basic_UPP_Details< ::DeferredTaskUPP,
	                                                    ::DeferredTaskProcPtr,
	                                                    ::NewDeferredTaskUPP,
	                                                    ::DisposeDeferredTaskUPP,
	                                                    ::InvokeDeferredTaskUPP >
	{};
	
	typedef UPP< DeferredTaskUPP_Details > DeferredTaskUPP;
	
	inline Owned< DeferredTaskUPP > NewDeferredTaskUPP( ::DeferredTaskProcPtr p )
	{
		return NewUPP< DeferredTaskUPP >( p );
	}
	
	inline void DisposeDeferredTaskUPP( Owned< DeferredTaskUPP > )  {}
	
	inline void InvokeDeferredTaskUPP( long             dtParam,
	                                   DeferredTaskUPP  userUPP )
	{
		userUPP( dtParam );
	}
	
	using ::DeferredTask;
	using ::DeferredTaskPtr;
	
	// ...
	
	void DTInstall( DeferredTask& dtTaskPtr );
	
	// ...
	
	inline UInt32 Delay( UInt32 ticks )
	{
		UInt32 finalTicks;
		::Delay( ticks, &finalTicks );
		return finalTicks;
	}
	
	// ...
	
}

#endif

