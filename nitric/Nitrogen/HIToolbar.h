//	HIToolbar.h
 
#ifndef NITROGEN_HITOOLBAR_H
#define NITROGEN_HITOOLBAR_H

#if !TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef __HITOOLBAR__
#include <HiToolbox/HIToolbar.h>
#endif

#ifndef NITROGEN_HIOBJECT_H
#include "Nitrogen/HIObject.h"
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

#ifndef NITROGEN_CFARRAY_H
#include "Nitrogen/CFArray.h"
#endif

#ifndef NITROGEN_MENUS_H
#include "Nitrogen/Menus.h"
#endif

#ifndef NITROGEN_CGIMAGE_H
#include "Nitrogen/CGImage.h"
#endif


/*
typedef HIObjectRef                     HIToolbarRef;
typedef HIObjectRef                     HIToolbarItemRef;
*/

namespace Nitrogen {

	void RegisterHIToolbarErrors();

/*==========================================================================*/
/* HITOOLBAR                                                                */
/*==========================================================================*/
	using ::HIToolbarRef;
	template<> struct DescType_Traits< typeHIToolbarRef		>: POD_DescType_Traits< HIToolbarRef > {};

	inline Owned<HIToolbarRef> HIToolbarCreate ( CFStringRef inIdentifier, ::OptionBits inOptions ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		HIToolbarRef result;
		ThrowOSStatus ( ::HIToolbarCreate ( inIdentifier, inOptions, &result ));
		return Owned<HIToolbarItemRef>::Seize( result );
		}

	inline OptionBits HIToolbarGetAttributes ( HIToolbarRef toolbar ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		::OptionBits result;
		ThrowOSStatus ( ::HIToolbarGetAttributes ( toolbar, &result ));
		return result;
		}

	inline void HIToolbarChangeAttributes ( HIToolbarRef inToolbar, ::OptionBits inAttrsToSet, ::OptionBits inAttrsToClear ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarChangeAttributes ( inToolbar, inAttrsToSet, inAttrsToClear ));
		}

	inline HIToolbarDisplayMode HIToolbarGetDisplayMode ( HIToolbarRef toolbar ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		HIToolbarDisplayMode result;
		ThrowOSStatus ( ::HIToolbarGetDisplayMode ( toolbar, &result ));
		return result;
		}

	inline void HIToolbarSetDisplayMode ( HIToolbarRef inToolbar, HIToolbarDisplayMode inDisplayMode ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarSetDisplayMode ( inToolbar, inDisplayMode ));
		}

	inline HIToolbarDisplaySize HIToolbarGetDisplaySize ( HIToolbarRef toolbar ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		HIToolbarDisplaySize result;
		ThrowOSStatus ( ::HIToolbarGetDisplayMode ( toolbar, &result ));
		return result;
		}

	inline void HIToolbarSetDisplaySize ( HIToolbarRef inToolbar, HIToolbarDisplaySize inSize ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarSetDisplaySize ( inToolbar, inSize ));
		}

	inline Owned<CFStringRef> HIToolbarCopyIdentifier ( HIToolbarRef inToolbar ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		CFStringRef result;
		ThrowOSStatus ( ::HIToolbarCopyIdentifier ( inToolbar, &result ));
		return Owned<CFStringRef>::Seize( result );
		}
		
	inline void HIToolbarSetItemsWithIdentifiers ( HIToolbarRef inToolbar, CFArrayRef inArray ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarSetItemsWithIdentifiers ( inToolbar, inArray ));
		}

	inline Owned<CFArrayRef> HIToolbarCopyItems ( HIToolbarRef inToolbar ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		CFArrayRef result;
		ThrowOSStatus ( ::HIToolbarCopyItems ( inToolbar, &result ));
		return Owned<CFArrayRef>::Seize( result );
		}
		
	inline Owned<HIToolbarItemRef> HIToolbarCreateItemWithIdentifier ( HIToolbarRef toolbar,
					 		CFStringRef inIdentifier, CFTypeRef inConfigData = NULL ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		HIToolbarItemRef result;
		ThrowOSStatus ( ::HIToolbarCreateItemWithIdentifier ( toolbar, inIdentifier, inConfigData, &result ));
		return Owned<HIToolbarItemRef>::Seize( result );
		}
		
	inline void HIToolbarInsertItemAtIndex ( HIToolbarRef inToolbar, HIToolbarItemRef inItem, CFIndex inIndex ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarInsertItemAtIndex ( inToolbar, inItem, inIndex ));
		}

	inline void HIToolbarAppendItem ( HIToolbarRef inToolbar, HIToolbarItemRef inItem ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarAppendItem ( inToolbar, inItem ));
		}

	inline void HIToolbarRemoveItemAtIndex ( HIToolbarRef inToolbar, CFIndex inIndex ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarRemoveItemAtIndex ( inToolbar, inIndex ));
		}

	inline void HIToolbarSetDelegate ( HIToolbarRef inToolbar, HIObjectRef inDelegate ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarSetDelegate ( inToolbar, inDelegate ));
		}

//	extern HIObjectRef HIToolbarGetDelegate(HIToolbarRef inToolbar)
	using ::HIToolbarGetDelegate;

/*==========================================================================*/
/* HITOOLBARITEMITEM                                                        */
/*==========================================================================*/
	using ::HIToolbarItemRef;
	template<> struct DescType_Traits< typeHIToolbarItemRef	>: POD_DescType_Traits< HIToolbarItemRef > {};

	inline Owned<HIToolbarItemRef> HIToolbarItemCreate ( CFStringRef inIdentifier, OptionBits inOptions ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		HIToolbarItemRef result;
		ThrowOSStatus ( ::HIToolbarItemCreate ( inIdentifier, inOptions, &result ));
		return Owned<HIToolbarItemRef>::Seize( result );
		}
		
	inline Owned<CFStringRef> HIToolbarItemCopyIdentifier ( HIToolbarItemRef inItem ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		CFStringRef result;
		ThrowOSStatus ( ::HIToolbarItemCopyIdentifier ( inItem, &result ));
		return Owned<CFStringRef>::Seize( result );
		}
		

	inline OptionBits HIToolbarItemGetAttributes ( HIToolbarItemRef inItem ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		::OptionBits result;
		ThrowOSStatus ( ::HIToolbarItemGetAttributes ( inItem, &result ));
		return result;
		}

	inline void HIToolbarItemChangeAttributes ( HIToolbarItemRef inItem, ::OptionBits inAttrsToSet, ::OptionBits inAttrsToClear ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarItemChangeAttributes ( inItem, inAttrsToSet, inAttrsToClear ));
		}

	inline void HIToolbarItemSetLabel ( HIToolbarItemRef inItem, CFStringRef inLabel ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarItemSetLabel ( inItem, inLabel ));
		}

	inline Owned<CFStringRef> HIToolbarItemCopyLabel ( HIToolbarItemRef inItem ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		CFStringRef result;
		ThrowOSStatus ( ::HIToolbarItemCopyLabel ( inItem, &result ));
		return Owned<CFStringRef>::Seize( result );
		}

	inline void HIToolbarItemSetHelpText ( HIToolbarItemRef inItem, CFStringRef inShortText, CFStringRef inLongText = NULL ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarItemSetHelpText ( inItem, inShortText, inLongText ));
		}

//	!!! This needs some ownership issues resolved
	inline void HIToolbarItemCopyHelpText ( HIToolbarItemRef inItem, CFStringRef *outShortText, CFStringRef *outLongText ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarItemCopyHelpText ( inItem, outShortText, outLongText ));
		}
	
//	Helper versions
	inline Owned<CFStringRef> HIToolbarItemCopyShortHelpText ( HIToolbarItemRef inItem ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		CFStringRef retVal;
		ThrowOSStatus ( ::HIToolbarItemCopyHelpText ( inItem, &retVal, NULL ));
		return Owned<CFStringRef>::Seize ( retVal );
		}

	inline Owned<CFStringRef> HIToolbarItemCopyLongHelpText ( HIToolbarItemRef inItem ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		CFStringRef retVal;
		ThrowOSStatus ( ::HIToolbarItemCopyHelpText ( inItem, NULL, &retVal ));
		return Owned<CFStringRef>::Seize ( retVal );
		}

	inline void HIToolbarItemSetCommandID ( HIToolbarItemRef inItem, ::MenuCommand inCommandID ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarItemSetCommandID ( inItem, inCommandID ));
		}
	
	inline MenuCommand HIToolbarItemGetCommandID ( HIToolbarItemRef inItem ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		::MenuCommand result;
		ThrowOSStatus ( ::HIToolbarItemGetCommandID ( inItem, &result ));
		return result;
		}

	inline void HIToolbarItemSetIconRef ( HIToolbarItemRef inItem, IconRef inIcon ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarItemSetIconRef ( inItem, inIcon ));
		}

	inline void HIToolbarItemSetImage ( HIToolbarItemRef inItem, CGImageRef inImage ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarItemSetImage ( inItem, inImage ));
		}

	inline Owned<CGImageRef> HIToolbarItemCopyImage ( HIToolbarItemRef inItem ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		CGImageRef result;
		ThrowOSStatus ( ::HIToolbarItemCopyImage ( inItem, &result ));
		return Owned<CGImageRef>::Seize( result );
		}

	inline void HIToolbarItemSetMenu ( HIToolbarItemRef inItem, MenuRef inMenu ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarItemSetMenu ( inItem, inMenu ));
		}
	
	inline Owned<MenuRef> HIToolbarItemCopyMenu ( HIToolbarItemRef inItem ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		MenuRef result;
		ThrowOSStatus ( ::HIToolbarItemCopyMenu ( inItem, &result ));
		return Owned<MenuRef>::Seize( result );
		}

//	extern HIToolbarRef HIToolbarItemGetToolbar(HIToolbarItemRef inItem)
	using ::HIToolbarItemGetToolbar;

//	extern Boolean HIToolbarItemIsEnabled(HIToolbarItemRef inItem)
	using ::HIToolbarItemIsEnabled;

	inline void HIToolbarItemSetEnabled ( HIToolbarItemRef inItem, Boolean inEnabled ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarItemSetEnabled ( inItem, inEnabled ));
		}
	
	inline void HIToolbarItemConfigDataChanged ( HIToolbarItemRef inItem ) {
     	OnlyOnce<RegisterHIToolbarErrors>();
		ThrowOSStatus ( ::HIToolbarItemConfigDataChanged ( inItem ));
		}

	}

#endif /* NITROGEN_HITOOLBAR_H */

