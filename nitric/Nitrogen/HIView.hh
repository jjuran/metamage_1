// Nitrogen/HIView.hh
// -------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2007 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_HIVIEW_HH
#define NITROGEN_HIVIEW_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __HIVIEW__
#include <Carbon/Carbon.h>
#endif

// Nitrogen
#ifndef MAC_TOOLBOX_TYPES_OPTIONBITS_HH
#include "Mac/Toolbox/Types/OptionBits.hh"
#endif
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif

#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif

#ifndef NITROGEN_CONTROLS_HH
#include "Nitrogen/Controls.hh"
#endif

#ifndef NITROGEN_ICONS_HH
#include "Nitrogen/Icons.hh"
#endif

#ifndef NITROGEN_CGIMAGE_HH
#include "Nitrogen/CGImage.hh"
#endif


namespace Nitrogen {

	class HIViewErrorsRegistrationDependency
	{
		public:
			HIViewErrorsRegistrationDependency();
	};
	
/*==============================================================================*/
/*  EMBEDDING                                                                   */
/*==============================================================================*/

#if ! __LP64__
	
/*	extern HIViewRef HIViewGetRoot(WindowRef inWindow) */
	using ::HIViewGetRoot;

	inline void HIViewAddSubview ( HIViewRef inParent, HIViewRef inNewChild ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewAddSubview ( inParent, inNewChild ));
		}
	
	inline void HIViewRemoveFromSuperview ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewRemoveFromSuperview ( inView ));
		}
	
/*	extern HIViewRef HIViewGetSuperview(HIViewRef inView) */
	using ::HIViewGetSuperview;

/*	extern HIViewRef HIViewGetFirstSubview(HIViewRef inView) */
	using ::HIViewGetFirstSubview;

/*	extern HIViewRef HIViewGetLastSubview(HIViewRef inView) */
	using ::HIViewGetLastSubview;
	
/*	extern HIViewRef HIViewGetNextView(HIViewRef inView) */
	using ::HIViewGetLastSubview;

/*	extern HIViewRef HIViewGetPreviousView(HIViewRef inView) */
	using ::HIViewGetPreviousView;

//	!!! Marshall says: there are iterators lurking in here
//	!!! waiting to be implemented

	inline void HIViewSetZOrder( HIViewRef inView, HIViewZOrderOp inOp, HIViewRef inOther = NULL ) {
		Mac::ThrowOSStatus ( ::HIViewSetZOrder ( inView, inOp, inOther ));
		}
	

/*==============================================================================*/
/*  VISIBILITY                                                                  */
/*==============================================================================*/

	inline void HIViewSetVisible ( HIViewRef inView, bool inVisible ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewSetVisible ( inView, inVisible ));
		}
	
/*	extern Boolean HIViewIsVisible(HIViewRef inView) */
	using ::HIViewIsVisible;


/*==============================================================================*/
/*  POSITIONING                                                                 */
/*==============================================================================*/

	inline HIRect HIViewGetBounds ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIViewGetBounds ( inView, &retVal ));
		return retVal;
		}
	
	inline HIRect HIViewGetFrame ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIViewGetFrame ( inView, &retVal ));
		return retVal;
		}
	
	inline void HIViewSetFrame ( HIViewRef inView, const HIRect &inRect ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewSetFrame ( inView, &inRect ));
		}
	
	inline void HIViewMoveBy ( HIViewRef inView, float inDX, float inDY ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewMoveBy ( inView, inDX, inDY ));
		}

	inline void HIViewPlaceInSuperviewAt ( HIViewRef inView, float inX, float inY ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewPlaceInSuperviewAt ( inView, inX, inY ));
		}

	inline void HIViewReshapeStructure ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewReshapeStructure ( inView ));
		}

#ifdef MAC_OS_X_VERSION_10_3

	inline void HIViewRegionChanged ( HIViewRef inView, ControlPartCode inRegionCode ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewRegionChanged ( inView, inRegionCode ));
		}

#endif

	
/*==============================================================================*/
/*  HIT TESTING/EVENT HANDLING                                                  */
/*==============================================================================*/

	inline HIViewRef HIViewGetViewForMouseEvent ( HIViewRef inView, EventRef inEvent ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HIViewGetViewForMouseEvent ( inView, inEvent, &retVal ));
		return retVal;
		}

	inline void HIViewClick (HIViewRef inView, EventRef inEvent ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewClick ( inView, inEvent ));
		}
	
	inline ControlPartCode HIViewSimulateClick ( HIViewRef inView, 
				HIViewPartCode inPartToClick, UInt32 inModifiers ) {
     	(void) HIViewErrorsRegistrationDependency();
		::ControlPartCode retVal;
		Mac::ThrowOSStatus ( ::HIViewSimulateClick ( inView, inPartToClick, inModifiers, &retVal ));
		return ControlPartCode( retVal );
		}
	
	inline HIViewPartCode HIViewGetPartHit ( HIViewRef inView, const HIPoint &inPoint ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewPartCode retVal;
		Mac::ThrowOSStatus ( ::HIViewGetPartHit ( inView, &inPoint, &retVal ));
		return retVal;
		}
	
	inline HIViewRef HIViewGetSubviewHit ( HIViewRef inView, 
							const HIPoint &inPoint, bool inDeep ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HIViewGetSubviewHit ( inView, &inPoint, inDeep, &retVal ));
		return retVal;
		}
	

/*==============================================================================*/
/*  DISPLAY                                                                     */
/*==============================================================================*/

/*	extern Boolean HIViewGetNeedsDisplay(HIViewRef inView) */
	using ::HIViewGetNeedsDisplay;
	
	inline void HIViewSetNeedsDisplay ( HIViewRef inView, bool inNeedsDisplay ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewSetNeedsDisplay ( inView, inNeedsDisplay ));
		}
	
	inline void HIViewSetNeedsDisplayInRegion ( HIViewRef inView, RgnHandle inRgn, bool inNeedsDisplay ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewSetNeedsDisplayInRegion ( inView, inRgn, inNeedsDisplay ));
		}

#ifdef MAC_OS_X_VERSION_10_3

	inline void HIViewRender ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewRender ( inView ));
		}

#endif

	inline void HIViewFlashDirtyArea ( WindowRef inWindow ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewFlashDirtyArea ( inWindow ));
		}

	struct HIViewGetSizeConstraints_Result {
		HISize	minSize;
		HISize	maxSize;
		};
	
	inline HIViewGetSizeConstraints_Result HIViewGetSizeConstraints ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewGetSizeConstraints_Result retVal;
		Mac::ThrowOSStatus ( ::HIViewGetSizeConstraints ( inView, &retVal.minSize, &retVal.maxSize ));
		return retVal;
		}


/*==============================================================================*/
/*  COORDINATE SYSTEM CONVERSION                                                */
/*==============================================================================*/

	inline void HIViewConvertPoint ( HIPoint &ioPoint, HIViewRef inSourceView, HIViewRef inDestView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewConvertPoint ( &ioPoint, inSourceView, inDestView ));
		}
	
	inline void HIViewConvertRect ( HIRect &ioRect, HIViewRef inSourceView, HIViewRef inDestView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewConvertRect ( &ioRect, inSourceView, inDestView ));
		}

	inline void HIViewConvertRegion ( RgnHandle ioRgn, HIViewRef inSourceView, HIViewRef inDestView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewConvertRegion ( ioRgn, inSourceView, inDestView ));
		}

#if 0
//	Returns a new, converted region. Original region is untouched
	inline nucleus::owned<RgnHandle> HIViewConvertRegion ( RgnHandle inRgn, HIViewRef inSourceView, HIViewRef inDestView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewConvertRegion ( ioRgn, inSourceView, inDestView ));
		}
#endif

	inline void HIViewSetDrawingEnabled ( HIViewRef inView, bool inEnabled ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewSetDrawingEnabled ( inView, inEnabled ));
		}

/*	extern Boolean HIViewIsDrawingEnabled(HIViewRef inView) */
	using ::HIViewSetDrawingEnabled;

	inline void HIViewScrollRect ( HIViewRef inView, const HIRect &inRect, float inDX, float inDY ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewScrollRect ( inView, &inRect, inDX, inDY ));
		}
		
//	Scroll the whole view
	inline void HIViewScrollRect ( HIViewRef inView, float inDX, float inDY ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewScrollRect ( inView, NULL, inDX, inDY ));
		}

	inline void HIViewSetBoundsOrigin ( HIViewRef inView, float inX, float inY ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewSetBoundsOrigin ( inView, inX, inY ));
		}


/*==============================================================================*/
/*  KEYBOARD FOCUS                                                              */
/*==============================================================================*/

	inline void HIViewAdvanceFocus ( HIViewRef inRootForFocus, EventModifiers inModifiers ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewAdvanceFocus ( inRootForFocus, inModifiers ));
		}
	
	inline HIViewPartCode HIViewGetFocusPart ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewPartCode retVal;
		Mac::ThrowOSStatus ( ::HIViewGetFocusPart ( inView, &retVal ));
		return retVal;
		}

/*	extern Boolean HIViewSubtreeContainsFocus(HIViewRef inSubtreeStart) */
	using ::HIViewSubtreeContainsFocus;

	inline void HIViewSetNextFocus ( HIViewRef inView, HIViewRef inNextFocus ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewSetNextFocus ( inView, inNextFocus ));
		}
	
	inline void HIViewSetFirstSubViewFocus ( HIViewRef inParent, HIViewRef inSubView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewSetFirstSubViewFocus ( inParent, inSubView ));
		}
	

/*==============================================================================*/
/*  LAYOUT                                                                      */
/*==============================================================================*/

#ifdef MAC_OS_X_VERSION_10_3

	inline HILayoutInfo HIViewGetLayoutInfo ( HIViewRef inView, UInt32 version = kHILayoutInfoVersionZero ) {
     	(void) HIViewErrorsRegistrationDependency();
		HILayoutInfo retVal;
		retVal.version = version;
		Mac::ThrowOSStatus ( ::HIViewGetLayoutInfo ( inView, &retVal ));
		return retVal;
		}

	inline void HIViewSetLayoutInfo ( HIViewRef inView, const HILayoutInfo &inLayoutInfo ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewSetLayoutInfo ( inView, &inLayoutInfo ));
		}
			
	inline void HIViewSuspendLayout ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewSuspendLayout ( inView ));
		}
	
	inline void HIViewResumeLayout ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewResumeLayout ( inView ));
		}

/*	extern Boolean HIViewIsLayoutActive(HIViewRef inView) */
	using ::HIViewIsLayoutActive;
	
	inline void HIViewApplyLayout ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewApplyLayout ( inView ));
		}

#endif

/*==============================================================================*/
/*  MISCELLANEOUS                                                               */
/*==============================================================================*/

#ifdef MAC_OS_X_VERSION_10_3

/*	extern WindowRef HIViewGetWindow(HIViewRef inView) */
	using ::HIViewGetWindow;

#endif

	inline HIViewRef HIViewFindByID ( HIViewRef inStartView, HIViewID inID ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HIViewFindByID ( inStartView, inID, &retVal ));
		return retVal;
		}

	inline HIViewRef HIViewFindByID ( WindowRef aWindow, HIViewID inID ) {
		return Nitrogen::HIViewFindByID ( HIViewGetRoot ( aWindow ), inID );
		}

	inline Mac::OptionBits HIViewGetAttributes ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		::OptionBits retVal;
		Mac::ThrowOSStatus ( ::HIViewGetAttributes ( inView, &retVal ));
		return Mac::OptionBits( retVal );
		}

	inline void HIViewChangeAttributes ( HIViewRef inView, Mac::OptionBits inAttrsToSet, Mac::OptionBits inAttrsToClear ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewChangeAttributes ( inView, inAttrsToSet, inAttrsToClear ));
		}

	struct HIViewCreateOffscreenImage_Result {
		HIRect frame;
 		nucleus::owned<CGImageRef> image;
		operator nucleus::owned<CGImageRef> () { return image; } /* non-const, returns owned */
		operator CGImageRef () const  { return image; }
 		};
	
	HIViewCreateOffscreenImage_Result HIViewCreateOffscreenImage ( HIViewRef inView, Mac::OptionBits inOptions = Mac::OptionBits() );

	inline void HIViewDrawCGImage ( CGContextRef inContext, const HIRect &inBounds, CGImageRef inImage ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewDrawCGImage ( inContext, &inBounds, inImage ));
		}
	
#ifdef MAC_OS_X_VERSION_10_3

	inline HIViewFeatures HIViewGetFeatures ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewFeatures retVal;
		Mac::ThrowOSStatus ( ::HIViewGetFeatures ( inView, &retVal ));
		return retVal;
		}

	inline void HIViewChangeFeatures ( HIViewRef inView, HIViewFeatures inFeaturesToSet, HIViewFeatures inFeaturesToClear ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIViewChangeFeatures ( inView, inFeaturesToSet, inFeaturesToClear ));
		}

#endif


/*==============================================================================*/
/* HIGrowBoxView                                                                */
/*==============================================================================*/

	inline void HIGrowBoxViewSetTransparent ( HIViewRef inGrowBoxView, bool inTransparent ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIGrowBoxViewSetTransparent ( inGrowBoxView, inTransparent ));
		}

/*	extern Boolean HIGrowBoxViewIsTransparent(HIViewRef inGrowBoxView) */
	using ::HIGrowBoxViewIsTransparent;
	

/*==============================================================================*/
/*  HIScrollView                                                                */
/*==============================================================================*/

	inline nucleus::owned<HIViewRef> HIScrollViewCreate ( Mac::OptionBits inOptions ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HIScrollViewCreate ( inOptions, &retVal ));
		return nucleus::owned<HIViewRef>::seize ( retVal );
		}

	inline void HIScrollViewSetScrollBarAutoHide ( HIViewRef inGrowBoxView, bool inAutoHide ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIScrollViewSetScrollBarAutoHide ( inGrowBoxView, inAutoHide ));
		}

/*	extern Boolean HIScrollViewGetScrollBarAutoHide(HIViewRef inView) */
	using ::HIScrollViewGetScrollBarAutoHide;
	
#ifdef MAC_OS_X_VERSION_10_3

	inline void HIScrollViewNavigate ( HIViewRef inGrowBoxView, HIScrollViewAction inAction ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIScrollViewNavigate ( inGrowBoxView, inAction ));
		}

/*	extern Boolean HIScrollViewCanNavigate(HIViewRef inView, HIScrollViewAction inAction) */
	using ::HIScrollViewCanNavigate;
	
#endif


/*==============================================================================*/
/*  HIImageView                                                                 */
/*==============================================================================*/

//	I think that this is a bug in the headers. This should return an HIViewRef, not a ControlRef
	inline nucleus::owned<HIViewRef> HIImageViewCreate ( CGImageRef inImage ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HIImageViewCreate ( inImage, &retVal ));
		return nucleus::owned<ControlRef>::seize ( retVal );
		}

	inline void HIImageViewSetOpaque ( HIViewRef inView, bool inOpaque ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIImageViewSetOpaque ( inView, inOpaque ));
		}

/*	extern Boolean HIImageViewIsOpaque(HIViewRef inView) */
	using ::HIImageViewSetOpaque;
	
	inline void HIImageViewSetAlpha ( HIViewRef inView, float inAlpha ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIImageViewSetAlpha ( inView, inAlpha ));
		}

/*	extern float HIImageViewGetAlpha(HIViewRef inView) */
	using ::HIImageViewGetAlpha;

	inline void HIImageViewSetScaleToFit ( HIViewRef inView, bool inScaleToFit ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIImageViewSetScaleToFit ( inView, inScaleToFit ));
		}

/*	extern Boolean HIImageViewGetScaleToFit(HIViewRef inView) */
	using ::HIImageViewGetScaleToFit;


	inline void HIImageViewSetImage ( HIViewRef inView, CGImageRef inImage ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIImageViewSetImage ( inView, inImage ));
		}

	inline nucleus::owned<CGImageRef> HIImageViewCopyImage ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		return nucleus::owned<CGImageRef>::seize ( ::HIImageViewCopyImage ( inView ));
		}


/*==============================================================================*/
/* HIComboBox                                                                   */
/*==============================================================================*/

//	Several variations
	inline nucleus::owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, Mac::OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, NULL, NULL, NULL, inAttributes, &retVal ));
		return nucleus::owned<HIViewRef>::seize ( retVal );
		}
	
	inline nucleus::owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, CFArrayRef list, Mac::OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, NULL, NULL, list, inAttributes, &retVal ));
		return nucleus::owned<HIViewRef>::seize ( retVal );
		}

	inline nucleus::owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, CFStringRef text, CFArrayRef list, Mac::OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, text, NULL, list, inAttributes, &retVal ));
		return nucleus::owned<HIViewRef>::seize ( retVal );
		}

	inline nucleus::owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, CFStringRef text, 
							const ControlFontStyleRec &style, CFArrayRef list, Mac::OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, text, &style, list, inAttributes, &retVal ));
		return nucleus::owned<HIViewRef>::seize ( retVal );
		}

/*	extern ItemCount HIComboBoxGetItemCount(HIViewRef inComboBox) */
	using ::HIComboBoxGetItemCount;
	
	inline void HIComboBoxInsertTextItemAtIndex ( HIViewRef inComboBox, CFIndex inIndex, CFStringRef inText ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIComboBoxInsertTextItemAtIndex ( inComboBox, inIndex, inText ));
		}

	inline CFIndex HIComboBoxAppendTextItem ( HIViewRef inComboBox, CFStringRef inText ) {
		CFIndex retVal;
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIComboBoxAppendTextItem ( inComboBox, inText, &retVal ));
		return retVal;
		}

	inline nucleus::owned<CFStringRef> HIComboBoxCopyTextItemAtIndex ( HIViewRef inComboBox, CFIndex inIndex ) {
		CFStringRef retVal;
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIComboBoxCopyTextItemAtIndex ( inComboBox, inIndex, &retVal ));
		return nucleus::owned<CFStringRef>::seize ( retVal );
		}

	inline void HIComboBoxRemoveItemAtIndex ( HIViewRef inComboBox, CFIndex inIndex ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIComboBoxRemoveItemAtIndex ( inComboBox, inIndex ));
		}

	inline void HIComboBoxChangeAttributes ( HIViewRef inComboBox, Mac::OptionBits inAttributesToSet, Mac::OptionBits inAttributesToClear ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIComboBoxChangeAttributes ( inComboBox, inAttributesToSet, inAttributesToClear ));
		}

	inline Mac::OptionBits HIComboBoxGetAttributes ( HIViewRef inComboBox ) {
     	(void) HIViewErrorsRegistrationDependency();
		::OptionBits retVal;
		Mac::ThrowOSStatus ( ::HIComboBoxGetAttributes ( inComboBox, &retVal ));
		return Mac::OptionBits( retVal );
		}


/*==============================================================================*/
/*  HISearchField                                                               */
/*==============================================================================*/

#ifdef MAC_OS_X_VERSION_10_3

	inline nucleus::owned<HIViewRef> HISearchFieldCreate ( Mac::OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HISearchFieldCreate ( NULL, inAttributes, NULL, NULL, &retVal ));
		return nucleus::owned<HIViewRef>::seize ( retVal );
		}
	
	inline nucleus::owned<HIViewRef> HISearchFieldCreate ( const HIRect &inBounds, Mac::OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HISearchFieldCreate ( &inBounds, inAttributes, NULL, NULL, &retVal ));
		return nucleus::owned<HIViewRef>::seize ( retVal );
		}

	inline nucleus::owned<HIViewRef> HISearchFieldCreate ( const HIRect &inBounds, Mac::OptionBits inAttributes,
							MenuRef inSearchMenu, CFStringRef inDescriptiveText ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HISearchFieldCreate ( &inBounds, inAttributes, inSearchMenu, inDescriptiveText, &retVal ));
		return nucleus::owned<HIViewRef>::seize ( retVal );
		}

	inline void HISearchFieldSetSearchMenu ( HIViewRef inSearchField, MenuRef inSearchMenu ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISearchFieldSetSearchMenu ( inSearchField, inSearchMenu ));
		}

	inline MenuRef HISearchFieldGetSearchMenu ( HIViewRef inSearchField ) {
     	(void) HIViewErrorsRegistrationDependency();
		MenuRef retVal;
		Mac::ThrowOSStatus ( ::HISearchFieldGetSearchMenu ( inSearchField, &retVal ));
		return retVal;
		}

	inline void HISearchFieldChangeAttributes ( HIViewRef inSearchField, Mac::OptionBits inAttributesToSet, Mac::OptionBits inAttributesToClear ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISearchFieldChangeAttributes ( inSearchField, inAttributesToSet, inAttributesToClear ));
		}

	inline Mac::OptionBits HISearchFieldGetAttributes ( HIViewRef inSearchField ) {
     	(void) HIViewErrorsRegistrationDependency();
		::OptionBits retVal;
		Mac::ThrowOSStatus ( ::HISearchFieldGetAttributes ( inSearchField, &retVal ));
		return Mac::OptionBits( retVal );
		}

	inline void HISearchFieldSetDescriptiveText ( HIViewRef inSearchField, CFStringRef inDescription ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISearchFieldSetDescriptiveText ( inSearchField, inDescription ));
		}

	inline nucleus::owned<CFStringRef> HISearchFieldCopyDescriptiveText ( HIViewRef inSearchField ) {
     	(void) HIViewErrorsRegistrationDependency();
		CFStringRef retVal;
		Mac::ThrowOSStatus ( ::HISearchFieldCopyDescriptiveText ( inSearchField, &retVal ));
		return nucleus::owned<CFStringRef>::seize ( retVal );
		}

#endif  // #ifdef MAC_OS_X_VERSION_10_3


/*==============================================================================*/
/*  HIMenuView                                                                  */
/*                                                                              */
/*  Because HIMenuView and HIStandardMenuView are not typically created         */
/*  directly by applications, no API is provided to create instances of these   */
/*  views. If you need to create an instance of either view, you can use        */
/*  HIObjectCreate.                                                             */
/*==============================================================================*/

#ifdef MAC_OS_X_VERSION_10_3

/*	extern MenuRef HIMenuViewGetMenu(HIViewRef inView) */
	using ::HIMenuViewGetMenu;

	inline HIViewRef HIMenuGetContentView ( MenuRef inMenu, ThemeMenuType inMenuType ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HIMenuGetContentView ( inMenu, inMenuType, &retVal ));
		return retVal;
		}

#endif


/*==============================================================================*/
/*  HISegmentedView                                                             */
/*==============================================================================*/

#ifdef MAC_OS_X_VERSION_10_3

	inline nucleus::owned<HIViewRef> HISegmentedViewCreate ( const HIRect &inBounds ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		Mac::ThrowOSStatus ( ::HISegmentedViewCreate ( &inBounds, &retVal ));
		return nucleus::owned<HIViewRef>::seize ( retVal );
		}

	inline void HISegmentedViewSetSegmentCount ( HIViewRef inSegmentedView, UInt32 inSegmentCount ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISegmentedViewSetSegmentCount ( inSegmentedView, inSegmentCount ));
		}

/*	extern UInt32 HISegmentedViewGetSegmentCount(HIViewRef inSegmentedView) */
	using ::HISegmentedViewGetSegmentCount;

	inline void HISegmentedViewSetSegmentBehavior ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, 
													HISegmentBehavior inBehavior ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISegmentedViewSetSegmentBehavior ( inSegmentedView, inSegmentIndexOneBased, inBehavior ));
		}

/*	extern HISegmentBehavior HISegmentedViewGetSegmentBehavior(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentBehavior;

	inline void HISegmentedViewChangeSegmentAttributes ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, 
					Mac::OptionBits inAttributesToSet, Mac::OptionBits inAttributesToClear ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISegmentedViewChangeSegmentAttributes ( inSegmentedView, inSegmentIndexOneBased, inAttributesToSet, inAttributesToClear ));
		}

/*	extern OptionBits HISegmentedViewGetSegmentAttributes(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentAttributes;

	inline void HISegmentedViewSetSegmentValue ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, SInt32 inValue ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISegmentedViewSetSegmentValue ( inSegmentedView, inSegmentIndexOneBased, inValue ));
		}

/*	extern HISegmentBehavior HISegmentedViewGetSegmentValue(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentValue;

	inline void HISegmentedViewSetSegmentEnabled ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, bool inEnabled ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISegmentedViewSetSegmentEnabled ( inSegmentedView, inSegmentIndexOneBased, inEnabled ));
		}

/*	extern HISegmentBehavior HISegmentedViewIsSegmentEnabled(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewIsSegmentEnabled;

	inline void HISegmentedViewSetSegmentCommand ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, UInt32 inCommand ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISegmentedViewSetSegmentCommand ( inSegmentedView, inSegmentIndexOneBased, inCommand ));
		}

/*	extern HISegmentBehavior HISegmentedViewGetSegmentCommand(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentCommand;

	inline void HISegmentedViewSetSegmentLabel ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, CFStringRef inLabel ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISegmentedViewSetSegmentLabel ( inSegmentedView, inSegmentIndexOneBased, inLabel ));
		}

	inline nucleus::owned<CFStringRef> HISegmentedViewCopySegmentLabel ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased ) {
     	(void) HIViewErrorsRegistrationDependency();
		CFStringRef retVal;
		Mac::ThrowOSStatus ( ::HISegmentedViewCopySegmentLabel ( inSegmentedView, inSegmentIndexOneBased, &retVal ));
		return nucleus::owned<CFStringRef>::seize ( retVal );
		}

//	Two versions here, one for "auto-width", the other for "explicit width".
	inline void HISegmentedViewSetSegmentContentWidth ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, bool inAutoCalculateWidth ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISegmentedViewSetSegmentContentWidth ( inSegmentedView, inSegmentIndexOneBased, inAutoCalculateWidth, 0.0 ));
		}

	inline void HISegmentedViewSetSegmentContentWidth ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, float inWidth ) {
     	(void) HIViewErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HISegmentedViewSetSegmentContentWidth ( inSegmentedView, inSegmentIndexOneBased, false, inWidth ));
		}

	struct HISegmentedViewGetSegmentContentWidth_Result {
		bool isAutoCalculated;
		float	segmentWidth;
		operator float () const { return segmentWidth; }
		};
		
	inline HISegmentedViewGetSegmentContentWidth_Result HISegmentedViewGetSegmentContentWidth ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased ) {
     	(void) HIViewErrorsRegistrationDependency();
		HISegmentedViewGetSegmentContentWidth_Result retVal;
		::Boolean tempVal;
		retVal.segmentWidth = ::HISegmentedViewGetSegmentContentWidth ( inSegmentedView, inSegmentIndexOneBased, &tempVal );
		retVal.isAutoCalculated = tempVal;
		return retVal;
		}

/*	extern HISegmentBehavior HISegmentedViewGetSegmentImageContentType(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentImageContentType;

//	The 10.3 headers say that a segmented image can only contain "no image", an IconRef, or a CGImageRef
//	So we specialize these three - 
	void HISegmentedViewSetSegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased );
	void HISegmentedViewSetSegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, IconRef iconRef );
	void HISegmentedViewSetSegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, CGImageRef imageRef );

#endif  // #ifdef MAC_OS_X_VERSION_10_3

#endif  // #if ! __LP64__
	
#if 0
	template <typename returnType>
	returnType HISegmentedViewCopySegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased );
	
	template < nucleus::owned<CGImageRef> > HISegmentedViewCopySegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased );
//	template < nucleus::owned<IconRef> >    HISegmentedViewCopySegmentImage;
#endif
	 }
	
#endif /* NITROGEN_HIVIEW_HH */
