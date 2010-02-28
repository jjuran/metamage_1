// Nitrogen/HIToolbar.hh
// ---------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2007 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.

 
#ifndef NITROGEN_HITOOLBAR_HH
#define NITROGEN_HITOOLBAR_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __HITOOLBAR__
#include <Carbon/Carbon.h>
#endif

#ifndef NITROGEN_HIOBJECT_HH
#include "Nitrogen/HIObject.hh"
#endif

#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif

#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_CFARRAY_HH
#include "Nitrogen/CFArray.hh"
#endif

#ifndef NITROGEN_MENUS_HH
#include "Nitrogen/Menus.hh"
#endif

#ifndef NITROGEN_CGIMAGE_HH
#include "Nitrogen/CGImage.hh"
#endif


/*
typedef HIObjectRef                     HIToolbarRef;
typedef HIObjectRef                     HIToolbarItemRef;
*/

namespace Nitrogen {

	class HIToolbarErrorsRegistrationDependency
	{
		public:
			HIToolbarErrorsRegistrationDependency();
	};
	
	inline HIToolbarErrorsRegistrationDependency::HIToolbarErrorsRegistrationDependency()
	{
		// Apple hasn't documented any HIToolbar errors.
	}
	
	static const EventParamType typeHIToolbarRef     = EventParamType( ::typeHIToolbarRef     );
	static const EventParamType typeHIToolbarItemRef = EventParamType( ::typeHIToolbarItemRef );
	
/*==========================================================================*/
/* HITOOLBAR                                                                */
/*==========================================================================*/
	using ::HIToolbarRef;
	template<> struct DescType_Traits< typeHIToolbarRef		>: Nucleus::PodFlattener< HIToolbarRef > {};

	inline Nucleus::Owned<HIToolbarRef> HIToolbarCreate ( CFStringRef inIdentifier, ::OptionBits inOptions ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		HIToolbarRef result;
		ThrowOSStatus ( ::HIToolbarCreate ( inIdentifier, inOptions, &result ));
		return Nucleus::Owned<HIToolbarItemRef>::Seize( result );
		}

	inline OptionBits HIToolbarGetAttributes ( HIToolbarRef toolbar ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		::OptionBits result;
		ThrowOSStatus ( ::HIToolbarGetAttributes ( toolbar, &result ));
		return OptionBits( result );
		}

	inline void HIToolbarChangeAttributes ( HIToolbarRef inToolbar, ::OptionBits inAttrsToSet, ::OptionBits inAttrsToClear ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarChangeAttributes ( inToolbar, inAttrsToSet, inAttrsToClear ));
		}

	inline HIToolbarDisplayMode HIToolbarGetDisplayMode ( HIToolbarRef toolbar ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		HIToolbarDisplayMode result;
		ThrowOSStatus ( ::HIToolbarGetDisplayMode ( toolbar, &result ));
		return result;
		}

	inline void HIToolbarSetDisplayMode ( HIToolbarRef inToolbar, HIToolbarDisplayMode inDisplayMode ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarSetDisplayMode ( inToolbar, inDisplayMode ));
		}

	inline HIToolbarDisplaySize HIToolbarGetDisplaySize ( HIToolbarRef toolbar ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		HIToolbarDisplaySize result;
		ThrowOSStatus ( ::HIToolbarGetDisplayMode ( toolbar, &result ));
		return result;
		}

	inline void HIToolbarSetDisplaySize ( HIToolbarRef inToolbar, HIToolbarDisplaySize inSize ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarSetDisplaySize ( inToolbar, inSize ));
		}

	inline Nucleus::Owned<CFStringRef> HIToolbarCopyIdentifier ( HIToolbarRef inToolbar ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFStringRef result;
		ThrowOSStatus ( ::HIToolbarCopyIdentifier ( inToolbar, &result ));
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}
		
	inline void HIToolbarSetItemsWithIdentifiers ( HIToolbarRef inToolbar, CFArrayRef inArray ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarSetItemsWithIdentifiers ( inToolbar, inArray ));
		}

	inline Nucleus::Owned<CFArrayRef> HIToolbarCopyItems ( HIToolbarRef inToolbar ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFArrayRef result;
		ThrowOSStatus ( ::HIToolbarCopyItems ( inToolbar, &result ));
		return Nucleus::Owned<CFArrayRef>::Seize( result );
		}
		
	inline Nucleus::Owned<HIToolbarItemRef> HIToolbarCreateItemWithIdentifier ( HIToolbarRef toolbar,
					 		CFStringRef inIdentifier, CFTypeRef inConfigData = NULL ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		HIToolbarItemRef result;
		ThrowOSStatus ( ::HIToolbarCreateItemWithIdentifier ( toolbar, inIdentifier, inConfigData, &result ));
		return Nucleus::Owned<HIToolbarItemRef>::Seize( result );
		}
		
	inline void HIToolbarInsertItemAtIndex ( HIToolbarRef inToolbar, HIToolbarItemRef inItem, CFIndex inIndex ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarInsertItemAtIndex ( inToolbar, inItem, inIndex ));
		}

	inline void HIToolbarAppendItem ( HIToolbarRef inToolbar, HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarAppendItem ( inToolbar, inItem ));
		}

	inline void HIToolbarRemoveItemAtIndex ( HIToolbarRef inToolbar, CFIndex inIndex ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarRemoveItemAtIndex ( inToolbar, inIndex ));
		}

	inline void HIToolbarSetDelegate ( HIToolbarRef inToolbar, HIObjectRef inDelegate ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarSetDelegate ( inToolbar, inDelegate ));
		}

//	extern HIObjectRef HIToolbarGetDelegate(HIToolbarRef inToolbar)
	using ::HIToolbarGetDelegate;

/*==========================================================================*/
/* HITOOLBARITEMITEM                                                        */
/*==========================================================================*/
	using ::HIToolbarItemRef;
	template<> struct DescType_Traits< typeHIToolbarItemRef	>: Nucleus::PodFlattener< HIToolbarItemRef > {};

	inline Nucleus::Owned<HIToolbarItemRef> HIToolbarItemCreate ( CFStringRef inIdentifier, OptionBits inOptions ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		HIToolbarItemRef result;
		ThrowOSStatus ( ::HIToolbarItemCreate ( inIdentifier, inOptions, &result ));
		return Nucleus::Owned<HIToolbarItemRef>::Seize( result );
		}
		
	inline Nucleus::Owned<CFStringRef> HIToolbarItemCopyIdentifier ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFStringRef result;
		ThrowOSStatus ( ::HIToolbarItemCopyIdentifier ( inItem, &result ));
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}
		

	inline OptionBits HIToolbarItemGetAttributes ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		::OptionBits result;
		ThrowOSStatus ( ::HIToolbarItemGetAttributes ( inItem, &result ));
		return OptionBits( result );
		}

	inline void HIToolbarItemChangeAttributes ( HIToolbarItemRef inItem, ::OptionBits inAttrsToSet, ::OptionBits inAttrsToClear ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarItemChangeAttributes ( inItem, inAttrsToSet, inAttrsToClear ));
		}

	inline void HIToolbarItemSetLabel ( HIToolbarItemRef inItem, CFStringRef inLabel ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarItemSetLabel ( inItem, inLabel ));
		}

	inline Nucleus::Owned<CFStringRef> HIToolbarItemCopyLabel ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFStringRef result;
		ThrowOSStatus ( ::HIToolbarItemCopyLabel ( inItem, &result ));
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}

	inline void HIToolbarItemSetHelpText ( HIToolbarItemRef inItem, CFStringRef inShortText, CFStringRef inLongText = NULL ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarItemSetHelpText ( inItem, inShortText, inLongText ));
		}

//	!!! This needs some ownership issues resolved
	inline void HIToolbarItemCopyHelpText ( HIToolbarItemRef inItem, CFStringRef *outShortText, CFStringRef *outLongText ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarItemCopyHelpText ( inItem, outShortText, outLongText ));
		}
	
//	Helper versions
	inline Nucleus::Owned<CFStringRef> HIToolbarItemCopyShortHelpText ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFStringRef retVal;
		ThrowOSStatus ( ::HIToolbarItemCopyHelpText ( inItem, &retVal, NULL ));
		return Nucleus::Owned<CFStringRef>::Seize ( retVal );
		}

	inline Nucleus::Owned<CFStringRef> HIToolbarItemCopyLongHelpText ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFStringRef retVal;
		ThrowOSStatus ( ::HIToolbarItemCopyHelpText ( inItem, NULL, &retVal ));
		return Nucleus::Owned<CFStringRef>::Seize ( retVal );
		}

	inline void HIToolbarItemSetCommandID ( HIToolbarItemRef inItem, ::MenuCommand inCommandID ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarItemSetCommandID ( inItem, inCommandID ));
		}
	
	inline MenuCommand HIToolbarItemGetCommandID ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		::MenuCommand result;
		ThrowOSStatus ( ::HIToolbarItemGetCommandID ( inItem, &result ));
		return MenuCommand( result );
		}

	inline void HIToolbarItemSetIconRef ( HIToolbarItemRef inItem, IconRef inIcon ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarItemSetIconRef ( inItem, inIcon ));
		}

	inline void HIToolbarItemSetImage ( HIToolbarItemRef inItem, CGImageRef inImage ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarItemSetImage ( inItem, inImage ));
		}

	inline Nucleus::Owned<CGImageRef> HIToolbarItemCopyImage ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CGImageRef result;
		ThrowOSStatus ( ::HIToolbarItemCopyImage ( inItem, &result ));
		return Nucleus::Owned<CGImageRef>::Seize( result );
		}

	inline void HIToolbarItemSetMenu ( HIToolbarItemRef inItem, MenuRef inMenu ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarItemSetMenu ( inItem, inMenu ));
		}
	
	inline Nucleus::Owned<MenuRef> HIToolbarItemCopyMenu ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		MenuRef result;
		ThrowOSStatus ( ::HIToolbarItemCopyMenu ( inItem, &result ));
		return Nucleus::Owned<MenuRef>::Seize( result );
		}

//	extern HIToolbarRef HIToolbarItemGetToolbar(HIToolbarItemRef inItem)
	using ::HIToolbarItemGetToolbar;

//	extern Boolean HIToolbarItemIsEnabled(HIToolbarItemRef inItem)
	using ::HIToolbarItemIsEnabled;

	inline void HIToolbarItemSetEnabled ( HIToolbarItemRef inItem, Boolean inEnabled ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarItemSetEnabled ( inItem, inEnabled ));
		}
	
	inline void HIToolbarItemConfigDataChanged ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIToolbarItemConfigDataChanged ( inItem ));
		}

	}

#endif /* NITROGEN_HITOOLBAR_HH */

