// Nitrogen/HIObject.hh
// --------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2007 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_HIOBJECT_HH
#define NITROGEN_HIOBJECT_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __HIOBJECT__
#include <Carbon/Carbon.h>
#endif

#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif

#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif

#ifndef NITROGEN_CARBONEVENTS_HH
#include "Nitrogen/CarbonEvents.hh"
#endif

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif

#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif


namespace Nitrogen {
	
	class HIObjectErrorsRegistrationDependency
	{
		public:
			HIObjectErrorsRegistrationDependency();
	};
	
	using ::HIObjectRef;

  }

namespace nucleus
  {
//	marshall sez: Dang! - we have to have our own disposer because we don't have "HIObjectGetTypeID"
//	template <> struct disposer_traits< HIObjectRef > : disposer_traits<CFTypeRef>  {};
//	template <> struct CFType_Traits< HIObjectRef > : Basic_CFType_Traits< CFDateRef, ::HIObjectGetTypeID > {};  }
	template <> struct disposer< HIObjectRef >
		{
		typedef HIObjectRef  argument_type;
		typedef void         result_type;
		
		void operator()( HIObjectRef hi ) const
			{
				::CFRelease ( hi );
			}
		};
  }

namespace Nitrogen
  {
	
	static const EventParamType typeHIObjectRef = EventParamType( ::typeHIObjectRef );
	
	template <> struct DescType_Traits< typeHIObjectRef >: nucleus::POD_scribe< HIObjectRef > {};

	using ::HIObjectClassRef;
  }

namespace nucleus
  {
   template <> struct disposer< HIObjectClassRef >
		{
		typedef HIObjectClassRef  argument_type;
		typedef void              result_type;
		
		void operator()( HIObjectClassRef hi ) const
			{
				(void) ::HIObjectUnregisterClass ( hi );
			}
		};
  }

namespace Nitrogen
  {

/*	As Lisa says, Level 0 */
	inline nucleus::owned<HIObjectClassRef> HIObjectRegisterSubclass (
			  CFStringRef            inClassID,
			  CFStringRef            inBaseClassID,
			  OptionBits             inOptions,
			  EventHandlerUPP        inConstructProc,       /* can be NULL */
			  UInt32                 inNumEvents,
			  const EventTypeSpec *  inEventList,
			  void *                 inConstructData = 0 ) {
		(void) HIObjectErrorsRegistrationDependency();
		HIObjectClassRef retVal;
		ThrowOSStatus ( ::HIObjectRegisterSubclass ( inClassID, inBaseClassID, inOptions,
	  						inConstructProc, inNumEvents, inEventList, inConstructData, &retVal ));
		return nucleus::owned<HIObjectClassRef>::seize( retVal );
		}
 

/*	Level 1 (UPP-creating) HIObjectRegisterSubclass */
	template < EventHandlerProcPtr handler >
	nucleus::owned<HIObjectClassRef> HIObjectRegisterSubclass(
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
	nucleus::owned<HIObjectClassRef> HIObjectRegisterSubclass (
			CFStringRef            inClassID,
			CFStringRef            inBaseClassID,
			OptionBits             inOptions,
			UInt32                 inNumEvents,
			const EventTypeSpec *  inEventList,
			typename nucleus::object_parameter_traits< Object >::type inUserData = typename nucleus::object_parameter_traits< Object >::type() ) {

		return HIObjectRegisterSubclass ( inClassID, inBaseClassID, inOptions,
							EventHandler_Bound_ObjectGlue< Object, handler >::UPP(),
							inNumEvents, inEventList, nucleus::object_parameter_traits< Object >::convert_to_pointer( inUserData ));
		}


//	inline void HIObjectUnregisterClass ( HIObjectClassRef inClassRef ) {
//		(void) HIObjectErrorsRegistrationDependency();
//		ThrowOSStatus ( ::HIObjectUnregisterClass ( inClassRef ));
//		}


	inline nucleus::owned<HIObjectRef> HIObjectCreate ( CFStringRef inClassID, EventRef inConstructData ) {
		(void) HIObjectErrorsRegistrationDependency();
		HIObjectRef retVal;
		ThrowOSStatus ( ::HIObjectCreate ( inClassID, inConstructData, &retVal ));
  	    return nucleus::owned<HIObjectRef>::seize( retVal );
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

	inline nucleus::owned<HIObjectRef> HIObjectCreateFromBundle ( CFBundleRef inBundle ) {
		(void) HIObjectErrorsRegistrationDependency();
		HIObjectRef retVal;
		ThrowOSStatus ( ::HIObjectCreateFromBundle ( inBundle, &retVal ));
  	    return nucleus::owned<HIObjectRef>::seize( retVal );
		}
	
/*	extern Boolean HIObjectIsAccessibilityIgnored(HIObjectRef inObject) */
	using ::HIObjectIsAccessibilityIgnored;

	inline void HIObjectSetAccessibilityIgnored ( HIObjectRef inObject, Boolean inIgnored ) {
		(void) HIObjectErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIObjectSetAccessibilityIgnored ( inObject, inIgnored ));
		}
	}

#endif /* NITROGEN_HIOBJECT_HH */

