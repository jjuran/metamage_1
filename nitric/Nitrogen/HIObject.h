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

#ifndef NITROGEN_CARBONEVENTS_H
#include "Nitrogen/CarbonEvents.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif

#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif

namespace Nitrogen {
	
	void RegisterHIObjectErrors ();
	
	using ::HIObjectRef;

  }

namespace Nucleus
  {
//	marshall sez: Dang! - we have to have our own Disposer because we don't have "HIObjectGetTypeID"
//	template <> struct OwnedDefaults< HIObjectRef > : OwnedDefaults<CFTypeRef>  {};
//	template <> struct CFType_Traits< HIObjectRef > : Basic_CFType_Traits< CFDateRef, ::HIObjectGetTypeID > {};  }
	template <> struct Disposer< Nitrogen::HIObjectRef >: public std::unary_function< Nitrogen::HIObjectRef, void >
		{
		void operator()( Nitrogen::HIObjectRef hi ) const
			{
				::CFRelease ( hi );
			}
		};
  }

namespace Nitrogen
  {

	template <> struct DescType_Traits< typeHIObjectRef >: Nucleus::PodFlattener< HIObjectRef > {};

	using ::HIObjectClassRef;
  }

namespace Nucleus
  {
   template <> struct Disposer< Nitrogen::HIObjectClassRef >: public std::unary_function< Nitrogen::HIObjectClassRef, void >
		{
		void operator()( Nitrogen::HIObjectClassRef hi ) const
			{
				(void) ::HIObjectUnregisterClass ( hi );
			}
		};
  }

namespace Nitrogen
  {

/*	As Lisa says, Level 0 */
	inline Nucleus::Owned<HIObjectClassRef> HIObjectRegisterSubclass (
			  CFStringRef            inClassID,
			  CFStringRef            inBaseClassID,
			  OptionBits             inOptions,
			  EventHandlerUPP        inConstructProc,       /* can be NULL */
			  UInt32                 inNumEvents,
			  const EventTypeSpec *  inEventList,
			  void *                 inConstructData = 0 ) {
		Nucleus::OnlyOnce<RegisterHIObjectErrors>();
		HIObjectClassRef retVal;
		ThrowOSStatus ( ::HIObjectRegisterSubclass ( inClassID, inBaseClassID, inOptions,
	  						inConstructProc, inNumEvents, inEventList, inConstructData, &retVal ));
		return Nucleus::Owned<HIObjectClassRef>::Seize( retVal );
		}
 

/*	Level 1 (UPP-creating) HIObjectRegisterSubclass */
	template < EventHandlerProcPtr handler >
	Nucleus::Owned<HIObjectClassRef> HIObjectRegisterSubclass(
			CFStringRef            inClassID,
			CFStringRef            inBaseClassID,
			OptionBits             inOptions,
			UInt32                 inNumEvents,
			const EventTypeSpec *  inEventList,
			void *                 inConstructData = 0 ) {
         return HIObjectRegisterSubclass( inClassID, inBaseClassID, inOptions, 
                                     StaticUPP< EventHandlerUPP, handler >(),
                                     inNumEvents,
                                     inEventList,
                                     inConstructData );
		}

/*	Level 2 (basic Nitrogen signature) HIObjectRegisterSubclass */
	template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
	Nucleus::Owned<HIObjectClassRef> HIObjectRegisterSubclass (
			CFStringRef            inClassID,
			CFStringRef            inBaseClassID,
			OptionBits             inOptions,
			UInt32                 inNumEvents,
			const EventTypeSpec *  inEventList,
			typename Nucleus::ObjectParameterTraits<Object>::Type inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() ) {

		return HIObjectRegisterSubclass ( inClassID, inBaseClassID, inOptions,
							EventHandler_Bound_ObjectGlue< Object, handler >::UPP(),
							inNumEvents, inEventList, Nucleus::ObjectParameterTraits<Object>::ConvertToPointer( inUserData ));
		}


//	inline void HIObjectUnregisterClass ( HIObjectClassRef inClassRef ) {
//		Nucleus::OnlyOnce<RegisterHIObjectErrors>();
//		ThrowOSStatus ( ::HIObjectUnregisterClass ( inClassRef ));
//		}


	inline Nucleus::Owned<HIObjectRef> HIObjectCreate ( CFStringRef inClassID, EventRef inConstructData ) {
		Nucleus::OnlyOnce<RegisterHIObjectErrors>();
		HIObjectRef retVal;
		ThrowOSStatus ( ::HIObjectCreate ( inClassID, inConstructData, &retVal ));
  	    return Nucleus::Owned<HIObjectRef>::Seize( retVal );
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

	inline Nucleus::Owned<HIObjectRef> HIObjectCreateFromBundle ( CFBundleRef inBundle ) {
		Nucleus::OnlyOnce<RegisterHIObjectErrors>();
		HIObjectRef retVal;
		ThrowOSStatus ( ::HIObjectCreateFromBundle ( inBundle, &retVal ));
  	    return Nucleus::Owned<HIObjectRef>::Seize( retVal );
		}
	
/*	extern Boolean HIObjectIsAccessibilityIgnored(HIObjectRef inObject) */
	using ::HIObjectIsAccessibilityIgnored;

	inline void HIObjectSetAccessibilityIgnored ( HIObjectRef inObject, Boolean inIgnored ) {
		Nucleus::OnlyOnce<RegisterHIObjectErrors>();
		ThrowOSStatus ( ::HIObjectSetAccessibilityIgnored ( inObject, inIgnored ));
		}
	}

#endif /* NITROGEN_HIOBJECT_H */

