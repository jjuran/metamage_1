#ifndef NITROGEN_HIOBJECT_H
#define NITROGEN_HIOBJECT_H

#if !TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef __HIOBJECT__
#include <HiToolbox/HIObject.h>
#endif

#ifndef	NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif

#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen {
	
	void RegisterHIObjectErrors ();
	
	using ::HIObjectRef;

//	marshall sez: Dang! - we have to have our own Disposer because we don't have "HIObjectGetTypeID"
//	template <> struct OwnedDefaults< HIObjectRef > : OwnedDefaults<CFTypeRef>  {};
//	template <> struct CFType_Traits< HIObjectRef > : Basic_CFType_Traits< CFDateRef, ::HIObjectGetTypeID > {};
	template <> struct Disposer< HIObjectRef >: public std::unary_function< HIObjectRef, void >
		{
		void operator()( HIObjectRef hi ) const
			{
				::CFRelease ( hi );
			}
		};

	template <> struct DescType_Traits< typeHIObjectRef >: POD_DescType_Traits< HIObjectRef > {};

	using ::HIObjectClassRef;
	template <> struct Disposer< HIObjectClassRef >: public std::unary_function< HIObjectClassRef, void >
		{
		void operator()( HIObjectClassRef hi ) const
			{
				(void) ::HIObjectUnregisterClass ( hi );
			}
		};


	inline Owned<HIObjectClassRef> HIObjectRegisterSubclass (
			  CFStringRef            inClassID,
			  CFStringRef            inBaseClassID,
			  OptionBits             inOptions,
			  EventHandlerUPP        inConstructProc,       /* can be NULL */
			  UInt32                 inNumEvents,
			  const EventTypeSpec *  inEventList,
			  void *                 inConstructData ) {
		OnlyOnce<RegisterHIObjectErrors>();
		HIObjectClassRef retVal;
		ThrowOSStatus ( ::HIObjectRegisterSubclass ( inClassID, inBaseClassID, inOptions,
	  						inConstructProc, inNumEvents, inEventList, inConstructData, &retVal ));
		return Owned<HIObjectClassRef>::Seize( retVal );
		}
 
	inline void HIObjectUnregisterClass ( HIObjectClassRef inClassRef ) {
		OnlyOnce<RegisterHIObjectErrors>();
		ThrowOSStatus ( ::HIObjectUnregisterClass ( inClassRef ));
		}


	inline Owned<HIObjectRef> HIObjectCreate ( CFStringRef inClassID, EventRef inConstructData ) {
		OnlyOnce<RegisterHIObjectErrors>();
		HIObjectRef retVal;
		ThrowOSStatus ( ::HIObjectCreate ( inClassID, inConstructData, &retVal ));
  	    return Owned<HIObjectRef>::Seize( retVal );
		}

/*	extern EventTargetRef HIObjectGetEventTarget(HIObjectRef inObject) */
	using ::HIObjectGetEventTarget;

/*	extern void HIObjectPrintDebugInfo(HIObjectRef inObject) */
	using ::HIObjectPrintDebugInfo;

/*	extern CFStringRef HIObjectCopyClassID(HIObjectRef inObject) */
	using ::HIObjectCopyClassID;

/*	extern Boolean HIObjectIsOfClass( HIObjectRef inObject, CFStringRef inObjectClassID) */
	using ::HIObjectIsOfClass;

/*	extern void * HIObjectDynamicCast(HIObjectRef inObject, CFStringRef inClassID) */
	using ::HIObjectDynamicCast;

	inline Owned<HIObjectRef> HIObjectCreateFromBundle ( CFBundleRef inBundle ) {
		OnlyOnce<RegisterHIObjectErrors>();
		HIObjectRef retVal;
		ThrowOSStatus ( ::HIObjectCreateFromBundle ( inBundle, &retVal ));
  	    return Owned<HIObjectRef>::Seize( retVal );
		}
	
/*	extern Boolean HIObjectIsAccessibilityIgnored(HIObjectRef inObject) */
	using ::HIObjectIsAccessibilityIgnored;

	inline void HIObjectSetAccessibilityIgnored ( HIObjectRef inObject, Boolean inIgnored ) {
		OnlyOnce<RegisterHIObjectErrors>();
		ThrowOSStatus ( ::HIObjectSetAccessibilityIgnored ( inObject, inIgnored ));
		}
	}

#endif /* NITROGEN_HIOBJECT_H */

