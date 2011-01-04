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

// Nitrogen
#ifndef MAC_TOOLBOX_TYPES_OPTIONBITS_HH
#include "Mac/Toolbox/Types/OptionBits.hh"
#endif
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif

#ifndef NITROGEN_HIOBJECT_HH
#include "Nitrogen/HIObject.hh"
#endif

#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
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
	
/*==========================================================================*/
/* HITOOLBAR                                                                */
/*==========================================================================*/
	using ::HIToolbarRef;

	inline nucleus::owned<HIToolbarRef> HIToolbarCreate ( CFStringRef inIdentifier, ::OptionBits inOptions ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		HIToolbarRef result;
		Mac::ThrowOSStatus ( ::HIToolbarCreate ( inIdentifier, inOptions, &result ));
		return nucleus::owned<HIToolbarItemRef>::seize( result );
		}

	inline Mac::OptionBits HIToolbarGetAttributes ( HIToolbarRef toolbar ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		::OptionBits result;
		Mac::ThrowOSStatus ( ::HIToolbarGetAttributes ( toolbar, &result ));
		return Mac::OptionBits( result );
		}

	inline void HIToolbarChangeAttributes ( HIToolbarRef inToolbar, ::OptionBits inAttrsToSet, ::OptionBits inAttrsToClear ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarChangeAttributes ( inToolbar, inAttrsToSet, inAttrsToClear ));
		}

	inline HIToolbarDisplayMode HIToolbarGetDisplayMode ( HIToolbarRef toolbar ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		HIToolbarDisplayMode result;
		Mac::ThrowOSStatus ( ::HIToolbarGetDisplayMode ( toolbar, &result ));
		return result;
		}

	inline void HIToolbarSetDisplayMode ( HIToolbarRef inToolbar, HIToolbarDisplayMode inDisplayMode ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarSetDisplayMode ( inToolbar, inDisplayMode ));
		}

	inline HIToolbarDisplaySize HIToolbarGetDisplaySize ( HIToolbarRef toolbar ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		HIToolbarDisplaySize result;
		Mac::ThrowOSStatus ( ::HIToolbarGetDisplayMode ( toolbar, &result ));
		return result;
		}

	inline void HIToolbarSetDisplaySize ( HIToolbarRef inToolbar, HIToolbarDisplaySize inSize ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarSetDisplaySize ( inToolbar, inSize ));
		}

	inline nucleus::owned<CFStringRef> HIToolbarCopyIdentifier ( HIToolbarRef inToolbar ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFStringRef result;
		Mac::ThrowOSStatus ( ::HIToolbarCopyIdentifier ( inToolbar, &result ));
		return nucleus::owned<CFStringRef>::seize( result );
		}
		
	inline void HIToolbarSetItemsWithIdentifiers ( HIToolbarRef inToolbar, CFArrayRef inArray ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarSetItemsWithIdentifiers ( inToolbar, inArray ));
		}

	inline nucleus::owned<CFArrayRef> HIToolbarCopyItems ( HIToolbarRef inToolbar ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFArrayRef result;
		Mac::ThrowOSStatus ( ::HIToolbarCopyItems ( inToolbar, &result ));
		return nucleus::owned<CFArrayRef>::seize( result );
		}
		
	inline nucleus::owned<HIToolbarItemRef> HIToolbarCreateItemWithIdentifier ( HIToolbarRef toolbar,
					 		CFStringRef inIdentifier, CFTypeRef inConfigData = NULL ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		HIToolbarItemRef result;
		Mac::ThrowOSStatus ( ::HIToolbarCreateItemWithIdentifier ( toolbar, inIdentifier, inConfigData, &result ));
		return nucleus::owned<HIToolbarItemRef>::seize( result );
		}
		
	inline void HIToolbarInsertItemAtIndex ( HIToolbarRef inToolbar, HIToolbarItemRef inItem, CFIndex inIndex ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarInsertItemAtIndex ( inToolbar, inItem, inIndex ));
		}

	inline void HIToolbarAppendItem ( HIToolbarRef inToolbar, HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarAppendItem ( inToolbar, inItem ));
		}

	inline void HIToolbarRemoveItemAtIndex ( HIToolbarRef inToolbar, CFIndex inIndex ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarRemoveItemAtIndex ( inToolbar, inIndex ));
		}

	inline void HIToolbarSetDelegate ( HIToolbarRef inToolbar, HIObjectRef inDelegate ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarSetDelegate ( inToolbar, inDelegate ));
		}

//	extern HIObjectRef HIToolbarGetDelegate(HIToolbarRef inToolbar)
	using ::HIToolbarGetDelegate;

/*==========================================================================*/
/* HITOOLBARITEMITEM                                                        */
/*==========================================================================*/
	using ::HIToolbarItemRef;

	inline nucleus::owned<HIToolbarItemRef> HIToolbarItemCreate ( CFStringRef inIdentifier, Mac::OptionBits inOptions ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		HIToolbarItemRef result;
		Mac::ThrowOSStatus ( ::HIToolbarItemCreate ( inIdentifier, inOptions, &result ));
		return nucleus::owned<HIToolbarItemRef>::seize( result );
		}
		
	inline nucleus::owned<CFStringRef> HIToolbarItemCopyIdentifier ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFStringRef result;
		Mac::ThrowOSStatus ( ::HIToolbarItemCopyIdentifier ( inItem, &result ));
		return nucleus::owned<CFStringRef>::seize( result );
		}
		

	inline Mac::OptionBits HIToolbarItemGetAttributes ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		::OptionBits result;
		Mac::ThrowOSStatus ( ::HIToolbarItemGetAttributes ( inItem, &result ));
		return Mac::OptionBits( result );
		}

	inline void HIToolbarItemChangeAttributes ( HIToolbarItemRef inItem, ::OptionBits inAttrsToSet, ::OptionBits inAttrsToClear ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarItemChangeAttributes ( inItem, inAttrsToSet, inAttrsToClear ));
		}

	inline void HIToolbarItemSetLabel ( HIToolbarItemRef inItem, CFStringRef inLabel ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarItemSetLabel ( inItem, inLabel ));
		}

	inline nucleus::owned<CFStringRef> HIToolbarItemCopyLabel ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFStringRef result;
		Mac::ThrowOSStatus ( ::HIToolbarItemCopyLabel ( inItem, &result ));
		return nucleus::owned<CFStringRef>::seize( result );
		}

	inline void HIToolbarItemSetHelpText ( HIToolbarItemRef inItem, CFStringRef inShortText, CFStringRef inLongText = NULL ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarItemSetHelpText ( inItem, inShortText, inLongText ));
		}

//	!!! This needs some ownership issues resolved
	inline void HIToolbarItemCopyHelpText ( HIToolbarItemRef inItem, CFStringRef *outShortText, CFStringRef *outLongText ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarItemCopyHelpText ( inItem, outShortText, outLongText ));
		}
	
//	Helper versions
	inline nucleus::owned<CFStringRef> HIToolbarItemCopyShortHelpText ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFStringRef retVal;
		Mac::ThrowOSStatus ( ::HIToolbarItemCopyHelpText ( inItem, &retVal, NULL ));
		return nucleus::owned<CFStringRef>::seize ( retVal );
		}

	inline nucleus::owned<CFStringRef> HIToolbarItemCopyLongHelpText ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CFStringRef retVal;
		Mac::ThrowOSStatus ( ::HIToolbarItemCopyHelpText ( inItem, NULL, &retVal ));
		return nucleus::owned<CFStringRef>::seize ( retVal );
		}

	inline void HIToolbarItemSetCommandID ( HIToolbarItemRef inItem, ::MenuCommand inCommandID ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarItemSetCommandID ( inItem, inCommandID ));
		}
	
	inline MenuCommand HIToolbarItemGetCommandID ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		::MenuCommand result;
		Mac::ThrowOSStatus ( ::HIToolbarItemGetCommandID ( inItem, &result ));
		return MenuCommand( result );
		}

	inline void HIToolbarItemSetIconRef ( HIToolbarItemRef inItem, IconRef inIcon ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarItemSetIconRef ( inItem, inIcon ));
		}

	inline void HIToolbarItemSetImage ( HIToolbarItemRef inItem, CGImageRef inImage ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarItemSetImage ( inItem, inImage ));
		}

	inline nucleus::owned<CGImageRef> HIToolbarItemCopyImage ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		CGImageRef result;
		Mac::ThrowOSStatus ( ::HIToolbarItemCopyImage ( inItem, &result ));
		return nucleus::owned<CGImageRef>::seize( result );
		}

	inline void HIToolbarItemSetMenu ( HIToolbarItemRef inItem, MenuRef inMenu ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarItemSetMenu ( inItem, inMenu ));
		}
	
	inline nucleus::owned<MenuRef> HIToolbarItemCopyMenu ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		MenuRef result;
		Mac::ThrowOSStatus ( ::HIToolbarItemCopyMenu ( inItem, &result ));
		return nucleus::owned<MenuRef>::seize( result );
		}

//	extern HIToolbarRef HIToolbarItemGetToolbar(HIToolbarItemRef inItem)
	using ::HIToolbarItemGetToolbar;

//	extern Boolean HIToolbarItemIsEnabled(HIToolbarItemRef inItem)
	using ::HIToolbarItemIsEnabled;

	inline void HIToolbarItemSetEnabled ( HIToolbarItemRef inItem, bool inEnabled ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarItemSetEnabled ( inItem, inEnabled ));
		}
	
	inline void HIToolbarItemConfigDataChanged ( HIToolbarItemRef inItem ) {
     	(void) HIToolbarErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIToolbarItemConfigDataChanged ( inItem ));
		}

	}

#endif /* NITROGEN_HITOOLBAR_HH */

