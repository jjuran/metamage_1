// Nitrogen/HIView.h
// -----------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2007 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_HIVIEW_H
#define NITROGEN_HIVIEW_H

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __HIVIEW__
#include <HiToolbox/HIView.h>
#endif

#ifndef	NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

#ifndef NITROGEN_CONTROLS_H
#include "Nitrogen/Controls.h"
#endif

#ifndef	NITROGEN_ICONS_H
#include "Nitrogen/Icons.h"
#endif

#ifndef NITROGEN_CGIMAGE_H
#include "Nitrogen/CGImage.h"
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

/*	extern HIViewRef HIViewGetRoot(WindowRef inWindow) */
	using ::HIViewGetRoot;

	inline void HIViewAddSubview ( HIViewRef inParent, HIViewRef inNewChild ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewAddSubview ( inParent, inNewChild ));
		}
	
	inline void HIViewRemoveFromSuperview ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewRemoveFromSuperview ( inView ));
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
		ThrowOSStatus ( ::HIViewSetZOrder ( inView, inOp, inOther ));
		}
	

/*==============================================================================*/
/*  VISIBILITY                                                                  */
/*==============================================================================*/

	inline void HIViewSetVisible ( HIViewRef inView, Boolean inVisible ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewSetVisible ( inView, inVisible ));
		}
	
/*	extern Boolean HIViewIsVisible(HIViewRef inView) */
	using ::HIViewIsVisible;


/*==============================================================================*/
/*  POSITIONING                                                                 */
/*==============================================================================*/

	inline HIRect HIViewGetBounds ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIRect retVal;
		ThrowOSStatus ( ::HIViewGetBounds ( inView, &retVal ));
		return retVal;
		}
	
	inline HIRect HIViewGetFrame ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIRect retVal;
		ThrowOSStatus ( ::HIViewGetFrame ( inView, &retVal ));
		return retVal;
		}
	
	inline void HIViewSetFrame ( HIViewRef inView, const HIRect &inRect ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewSetFrame ( inView, &inRect ));
		}
	
	inline void HIViewMoveBy ( HIViewRef inView, float inDX, float inDY ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewMoveBy ( inView, inDX, inDY ));
		}

	inline void HIViewPlaceInSuperviewAt ( HIViewRef inView, float inX, float inY ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewPlaceInSuperviewAt ( inView, inX, inY ));
		}

	inline void HIViewReshapeStructure ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewReshapeStructure ( inView ));
		}

	inline void HIViewRegionChanged ( HIViewRef inView, ControlPartCode inRegionCode ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewRegionChanged ( inView, inRegionCode ));
		}

	
/*==============================================================================*/
/*  HIT TESTING/EVENT HANDLING                                                  */
/*==============================================================================*/

	inline HIViewRef HIViewGetViewForMouseEvent ( HIViewRef inView, EventRef inEvent ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIViewGetViewForMouseEvent ( inView, inEvent, &retVal ));
		return retVal;
		}

	inline void HIViewClick (HIViewRef inView, EventRef inEvent ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewClick ( inView, inEvent ));
		}
	
	inline ControlPartCode HIViewSimulateClick ( HIViewRef inView, 
				HIViewPartCode inPartToClick, UInt32 inModifiers ) {
     	(void) HIViewErrorsRegistrationDependency();
		::ControlPartCode retVal;
		ThrowOSStatus ( ::HIViewSimulateClick ( inView, inPartToClick, inModifiers, &retVal ));
		return ControlPartCode( retVal );
		}
	
	inline HIViewPartCode HIViewGetPartHit ( HIViewRef inView, const HIPoint &inPoint ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewPartCode retVal;
		ThrowOSStatus ( ::HIViewGetPartHit ( inView, &inPoint, &retVal ));
		return retVal;
		}
	
	inline HIViewRef HIViewGetSubviewHit ( HIViewRef inView, 
							const HIPoint &inPoint, Boolean inDeep ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIViewGetSubviewHit ( inView, &inPoint, inDeep, &retVal ));
		return retVal;
		}
	

/*==============================================================================*/
/*  DISPLAY                                                                     */
/*==============================================================================*/

/*	extern Boolean HIViewGetNeedsDisplay(HIViewRef inView) */
	using ::HIViewGetNeedsDisplay;
	
	inline void HIViewSetNeedsDisplay ( HIViewRef inView, Boolean inNeedsDisplay ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewSetNeedsDisplay ( inView, inNeedsDisplay ));
		}
	
	inline void HIViewSetNeedsDisplayInRegion ( HIViewRef inView, RgnHandle inRgn, Boolean inNeedsDisplay ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewSetNeedsDisplayInRegion ( inView, inRgn, inNeedsDisplay ));
		}
	
	inline void HIViewRender ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewRender ( inView ));
		}

	inline void HIViewFlashDirtyArea ( WindowRef inWindow ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewFlashDirtyArea ( inWindow ));
		}

	struct HIViewGetSizeConstraints_Result {
		HISize	minSize;
		HISize	maxSize;
		};
	
	inline HIViewGetSizeConstraints_Result HIViewGetSizeConstraints ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewGetSizeConstraints_Result retVal;
		ThrowOSStatus ( ::HIViewGetSizeConstraints ( inView, &retVal.minSize, &retVal.maxSize ));
		return retVal;
		}


/*==============================================================================*/
/*  COORDINATE SYSTEM CONVERSION                                                */
/*==============================================================================*/

	inline void HIViewConvertPoint ( HIPoint &ioPoint, HIViewRef inSourceView, HIViewRef inDestView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewConvertPoint ( &ioPoint, inSourceView, inDestView ));
		}
	
	inline void HIViewConvertRect ( HIRect &ioRect, HIViewRef inSourceView, HIViewRef inDestView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewConvertRect ( &ioRect, inSourceView, inDestView ));
		}

	inline void HIViewConvertRegion ( RgnHandle ioRgn, HIViewRef inSourceView, HIViewRef inDestView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewConvertRegion ( ioRgn, inSourceView, inDestView ));
		}

#if 0
//	Returns a new, converted region. Original region is untouched
	inline Nucleus::Owned<RgnHandle> HIViewConvertRegion ( RgnHandle inRgn, HIViewRef inSourceView, HIViewRef inDestView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewConvertRegion ( ioRgn, inSourceView, inDestView ));
		}
#endif

	inline void HIViewSetDrawingEnabled ( HIViewRef inView, Boolean inEnabled ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewSetDrawingEnabled ( inView, inEnabled ));
		}

/*	extern Boolean HIViewIsDrawingEnabled(HIViewRef inView) */
	using ::HIViewSetDrawingEnabled;

	inline void HIViewScrollRect ( HIViewRef inView, const HIRect &inRect, float inDX, float inDY ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewScrollRect ( inView, &inRect, inDX, inDY ));
		}
		
//	Scroll the whole view
	inline void HIViewScrollRect ( HIViewRef inView, float inDX, float inDY ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewScrollRect ( inView, NULL, inDX, inDY ));
		}

	inline void HIViewSetBoundsOrigin ( HIViewRef inView, float inX, float inY ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewSetBoundsOrigin ( inView, inX, inY ));
		}


/*==============================================================================*/
/*  KEYBOARD FOCUS                                                              */
/*==============================================================================*/

	inline void HIViewAdvanceFocus ( HIViewRef inRootForFocus, EventModifiers inModifiers ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewAdvanceFocus ( inRootForFocus, inModifiers ));
		}
	
	inline HIViewPartCode HIViewGetFocusPart ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewPartCode retVal;
		ThrowOSStatus ( ::HIViewGetFocusPart ( inView, &retVal ));
		return retVal;
		}

/*	extern Boolean HIViewSubtreeContainsFocus(HIViewRef inSubtreeStart) */
	using ::HIViewSubtreeContainsFocus;

	inline void HIViewSetNextFocus ( HIViewRef inView, HIViewRef inNextFocus ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewSetNextFocus ( inView, inNextFocus ));
		}
	
	inline void HIViewSetFirstSubViewFocus ( HIViewRef inParent, HIViewRef inSubView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewSetFirstSubViewFocus ( inParent, inSubView ));
		}
	

/*==============================================================================*/
/*  LAYOUT                                                                      */
/*==============================================================================*/

	inline HILayoutInfo HIViewGetLayoutInfo ( HIViewRef inView, UInt32 version = kHILayoutInfoVersionZero ) {
     	(void) HIViewErrorsRegistrationDependency();
		HILayoutInfo retVal;
		retVal.version = version;
		ThrowOSStatus ( ::HIViewGetLayoutInfo ( inView, &retVal ));
		return retVal;
		}

	inline void HIViewSetLayoutInfo ( HIViewRef inView, const HILayoutInfo &inLayoutInfo ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewSetLayoutInfo ( inView, &inLayoutInfo ));
		}
			
	inline void HIViewSuspendLayout ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewSuspendLayout ( inView ));
		}
	
	inline void HIViewResumeLayout ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewResumeLayout ( inView ));
		}

/*	extern Boolean HIViewIsLayoutActive(HIViewRef inView) */
	using ::HIViewIsLayoutActive;
	
	inline void HIViewApplyLayout ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewApplyLayout ( inView ));
		}

/*==============================================================================*/
/*  MISCELLANEOUS                                                               */
/*==============================================================================*/

/*	extern WindowRef HIViewGetWindow(HIViewRef inView) */
	using ::HIViewGetWindow;

	inline HIViewRef HIViewFindByID ( HIViewRef inStartView, HIViewID inID ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIViewFindByID ( inStartView, inID, &retVal ));
		return retVal;
		}

	inline HIViewRef HIViewFindByID ( WindowRef aWindow, HIViewID inID ) {
		return Nitrogen::HIViewFindByID ( HIViewGetRoot ( aWindow ), inID );
		}

	inline OptionBits HIViewGetAttributes ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		::OptionBits retVal;
		ThrowOSStatus ( ::HIViewGetAttributes ( inView, &retVal ));
		return OptionBits( retVal );
		}

	inline void HIViewChangeAttributes ( HIViewRef inView, OptionBits inAttrsToSet, OptionBits inAttrsToClear ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewChangeAttributes ( inView, inAttrsToSet, inAttrsToClear ));
		}

	struct HIViewCreateOffscreenImage_Result {
		HIRect frame;
 		Nucleus::Owned<CGImageRef> image;
		operator Nucleus::Owned<CGImageRef> () { return image; } /* non-const, returns owned */
		operator CGImageRef () const  { return image; }
 		};
	
	HIViewCreateOffscreenImage_Result HIViewCreateOffscreenImage ( HIViewRef inView, OptionBits inOptions = OptionBits() );

	inline void HIViewDrawCGImage ( CGContextRef inContext, const HIRect &inBounds, CGImageRef inImage ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewDrawCGImage ( inContext, &inBounds, inImage ));
		}
	
	inline HIViewFeatures HIViewGetFeatures ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewFeatures retVal;
		ThrowOSStatus ( ::HIViewGetFeatures ( inView, &retVal ));
		return retVal;
		}

	inline void HIViewChangeFeatures ( HIViewRef inView, HIViewFeatures inFeaturesToSet, HIViewFeatures inFeaturesToClear ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIViewChangeFeatures ( inView, inFeaturesToSet, inFeaturesToClear ));
		}


/*==============================================================================*/
/* HIGrowBoxView                                                                */
/*==============================================================================*/

	inline void HIGrowBoxViewSetTransparent ( HIViewRef inGrowBoxView, Boolean inTransparent ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIGrowBoxViewSetTransparent ( inGrowBoxView, inTransparent ));
		}

/*	extern Boolean HIGrowBoxViewIsTransparent(HIViewRef inGrowBoxView) */
	using ::HIGrowBoxViewIsTransparent;
	

/*==============================================================================*/
/*  HIScrollView                                                                */
/*==============================================================================*/

	inline Nucleus::Owned<HIViewRef> HIScrollViewCreate ( OptionBits inOptions ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIScrollViewCreate ( inOptions, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline void HIScrollViewSetScrollBarAutoHide ( HIViewRef inGrowBoxView, Boolean inAutoHide ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIScrollViewSetScrollBarAutoHide ( inGrowBoxView, inAutoHide ));
		}

/*	extern Boolean HIScrollViewGetScrollBarAutoHide(HIViewRef inView) */
	using ::HIScrollViewGetScrollBarAutoHide;
	
	inline void HIScrollViewNavigate ( HIViewRef inGrowBoxView, HIScrollViewAction inAction ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIScrollViewNavigate ( inGrowBoxView, inAction ));
		}

/*	extern Boolean HIScrollViewCanNavigate(HIViewRef inView, HIScrollViewAction inAction) */
	using ::HIScrollViewCanNavigate;
	

/*==============================================================================*/
/*  HIImageView                                                                 */
/*==============================================================================*/

//	I think that this is a bug in the headers. This should return an HIViewRef, not a ControlRef
	inline Nucleus::Owned<HIViewRef> HIImageViewCreate ( CGImageRef inImage ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIImageViewCreate ( inImage, &retVal ));
		return Nucleus::Owned<ControlRef>::Seize ( retVal );
		}

	inline void HIImageViewSetOpaque ( HIViewRef inView, Boolean inOpaque ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIImageViewSetOpaque ( inView, inOpaque ));
		}

/*	extern Boolean HIImageViewIsOpaque(HIViewRef inView) */
	using ::HIImageViewSetOpaque;
	
	inline void HIImageViewSetAlpha ( HIViewRef inView, float inAlpha ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIImageViewSetAlpha ( inView, inAlpha ));
		}

/*	extern float HIImageViewGetAlpha(HIViewRef inView) */
	using ::HIImageViewGetAlpha;

	inline void HIImageViewSetScaleToFit ( HIViewRef inView, Boolean inScaleToFit ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIImageViewSetScaleToFit ( inView, inScaleToFit ));
		}

/*	extern Boolean HIImageViewGetScaleToFit(HIViewRef inView) */
	using ::HIImageViewGetScaleToFit;


	inline void HIImageViewSetImage ( HIViewRef inView, CGImageRef inImage ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIImageViewSetImage ( inView, inImage ));
		}

	inline Nucleus::Owned<CGImageRef> HIImageViewCopyImage ( HIViewRef inView ) {
     	(void) HIViewErrorsRegistrationDependency();
		return Nucleus::Owned<CGImageRef>::Seize ( ::HIImageViewCopyImage ( inView ));
		}


/*==============================================================================*/
/* HIComboBox                                                                   */
/*==============================================================================*/

//	Several variations
	inline Nucleus::Owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, NULL, NULL, NULL, inAttributes, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}
	
	inline Nucleus::Owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, CFArrayRef list, OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, NULL, NULL, list, inAttributes, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline Nucleus::Owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, CFStringRef text, CFArrayRef list, OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, text, NULL, list, inAttributes, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline Nucleus::Owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, CFStringRef text, 
							const ControlFontStyleRec &style, CFArrayRef list, OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, text, &style, list, inAttributes, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

/*	extern ItemCount HIComboBoxGetItemCount(HIViewRef inComboBox) */
	using ::HIComboBoxGetItemCount;
	
	inline void HIComboBoxInsertTextItemAtIndex ( HIViewRef inComboBox, CFIndex inIndex, CFStringRef inText ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIComboBoxInsertTextItemAtIndex ( inComboBox, inIndex, inText ));
		}

	inline CFIndex HIComboBoxAppendTextItem ( HIViewRef inComboBox, CFStringRef inText ) {
		CFIndex retVal;
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIComboBoxAppendTextItem ( inComboBox, inText, &retVal ));
		return retVal;
		}

	inline Nucleus::Owned<CFStringRef> HIComboBoxCopyTextItemAtIndex ( HIViewRef inComboBox, CFIndex inIndex ) {
		CFStringRef retVal;
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIComboBoxCopyTextItemAtIndex ( inComboBox, inIndex, &retVal ));
		return Nucleus::Owned<CFStringRef>::Seize ( retVal );
		}

	inline void HIComboBoxRemoveItemAtIndex ( HIViewRef inComboBox, CFIndex inIndex ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIComboBoxRemoveItemAtIndex ( inComboBox, inIndex ));
		}

	inline void HIComboBoxChangeAttributes ( HIViewRef inComboBox, OptionBits inAttributesToSet, OptionBits inAttributesToClear ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HIComboBoxChangeAttributes ( inComboBox, inAttributesToSet, inAttributesToClear ));
		}

	inline OptionBits HIComboBoxGetAttributes ( HIViewRef inComboBox ) {
     	(void) HIViewErrorsRegistrationDependency();
		::OptionBits retVal;
		ThrowOSStatus ( ::HIComboBoxGetAttributes ( inComboBox, &retVal ));
		return OptionBits( retVal );
		}


/*==============================================================================*/
/*  HISearchField                                                               */
/*==============================================================================*/

	inline Nucleus::Owned<HIViewRef> HISearchFieldCreate ( OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HISearchFieldCreate ( NULL, inAttributes, NULL, NULL, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}
	
	inline Nucleus::Owned<HIViewRef> HISearchFieldCreate ( const HIRect &inBounds, OptionBits inAttributes ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HISearchFieldCreate ( &inBounds, inAttributes, NULL, NULL, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline Nucleus::Owned<HIViewRef> HISearchFieldCreate ( const HIRect &inBounds, OptionBits inAttributes,
							MenuRef inSearchMenu, CFStringRef inDescriptiveText ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HISearchFieldCreate ( &inBounds, inAttributes, inSearchMenu, inDescriptiveText, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline void HISearchFieldSetSearchMenu ( HIViewRef inSearchField, MenuRef inSearchMenu ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISearchFieldSetSearchMenu ( inSearchField, inSearchMenu ));
		}

	inline MenuRef HISearchFieldGetSearchMenu ( HIViewRef inSearchField ) {
     	(void) HIViewErrorsRegistrationDependency();
		MenuRef retVal;
		ThrowOSStatus ( ::HISearchFieldGetSearchMenu ( inSearchField, &retVal ));
		return retVal;
		}

	inline void HISearchFieldChangeAttributes ( HIViewRef inSearchField, OptionBits inAttributesToSet, OptionBits inAttributesToClear ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISearchFieldChangeAttributes ( inSearchField, inAttributesToSet, inAttributesToClear ));
		}

	inline OptionBits HISearchFieldGetAttributes ( HIViewRef inSearchField ) {
     	(void) HIViewErrorsRegistrationDependency();
		::OptionBits retVal;
		ThrowOSStatus ( ::HISearchFieldGetAttributes ( inSearchField, &retVal ));
		return OptionBits( retVal );
		}

	inline void HISearchFieldSetDescriptiveText ( HIViewRef inSearchField, CFStringRef inDescription ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISearchFieldSetDescriptiveText ( inSearchField, inDescription ));
		}

	inline Nucleus::Owned<CFStringRef> HISearchFieldCopyDescriptiveText ( HIViewRef inSearchField ) {
     	(void) HIViewErrorsRegistrationDependency();
		CFStringRef retVal;
		ThrowOSStatus ( ::HISearchFieldCopyDescriptiveText ( inSearchField, &retVal ));
		return Nucleus::Owned<CFStringRef>::Seize ( retVal );
		}


/*==============================================================================*/
/*  HIMenuView                                                                  */
/*                                                                              */
/*  Because HIMenuView and HIStandardMenuView are not typically created         */
/*  directly by applications, no API is provided to create instances of these   */
/*  views. If you need to create an instance of either view, you can use        */
/*  HIObjectCreate.                                                             */
/*==============================================================================*/

/*	extern MenuRef HIMenuViewGetMenu(HIViewRef inView) */
	using ::HIMenuViewGetMenu;

	inline HIViewRef HIMenuGetContentView ( MenuRef inMenu, ThemeMenuType inMenuType ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIMenuGetContentView ( inMenu, inMenuType, &retVal ));
		return retVal;
		}


/*==============================================================================*/
/*  HISegmentedView                                                             */
/*==============================================================================*/

	inline Nucleus::Owned<HIViewRef> HISegmentedViewCreate ( const HIRect &inBounds ) {
     	(void) HIViewErrorsRegistrationDependency();
		HIViewRef retVal;
		ThrowOSStatus ( ::HISegmentedViewCreate ( &inBounds, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline void HISegmentedViewSetSegmentCount ( HIViewRef inSegmentedView, UInt32 inSegmentCount ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentCount ( inSegmentedView, inSegmentCount ));
		}

/*	extern UInt32 HISegmentedViewGetSegmentCount(HIViewRef inSegmentedView) */
	using ::HISegmentedViewGetSegmentCount;

	inline void HISegmentedViewSetSegmentBehavior ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, 
													HISegmentBehavior inBehavior ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentBehavior ( inSegmentedView, inSegmentIndexOneBased, inBehavior ));
		}

/*	extern HISegmentBehavior HISegmentedViewGetSegmentBehavior(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentBehavior;

	inline void HISegmentedViewChangeSegmentAttributes ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, 
					OptionBits inAttributesToSet, OptionBits inAttributesToClear ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISegmentedViewChangeSegmentAttributes ( inSegmentedView, inSegmentIndexOneBased, inAttributesToSet, inAttributesToClear ));
		}

/*	extern OptionBits HISegmentedViewGetSegmentAttributes(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentAttributes;

	inline void HISegmentedViewSetSegmentValue ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, SInt32 inValue ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentValue ( inSegmentedView, inSegmentIndexOneBased, inValue ));
		}

/*	extern HISegmentBehavior HISegmentedViewGetSegmentValue(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentValue;

	inline void HISegmentedViewSetSegmentEnabled ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, Boolean inEnabled ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentEnabled ( inSegmentedView, inSegmentIndexOneBased, inEnabled ));
		}

/*	extern HISegmentBehavior HISegmentedViewIsSegmentEnabled(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewIsSegmentEnabled;

	inline void HISegmentedViewSetSegmentCommand ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, UInt32 inCommand ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentCommand ( inSegmentedView, inSegmentIndexOneBased, inCommand ));
		}

/*	extern HISegmentBehavior HISegmentedViewGetSegmentCommand(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentCommand;

	inline void HISegmentedViewSetSegmentLabel ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, CFStringRef inLabel ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentLabel ( inSegmentedView, inSegmentIndexOneBased, inLabel ));
		}

	inline Nucleus::Owned<CFStringRef> HISegmentedViewCopySegmentLabel ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased ) {
     	(void) HIViewErrorsRegistrationDependency();
		CFStringRef retVal;
		ThrowOSStatus ( ::HISegmentedViewCopySegmentLabel ( inSegmentedView, inSegmentIndexOneBased, &retVal ));
		return Nucleus::Owned<CFStringRef>::Seize ( retVal );
		}

//	Two versions here, one for "auto-width", the other for "explicit width".
	inline void HISegmentedViewSetSegmentContentWidth ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, Boolean inAutoCalculateWidth ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentContentWidth ( inSegmentedView, inSegmentIndexOneBased, inAutoCalculateWidth, 0.0 ));
		}

	inline void HISegmentedViewSetSegmentContentWidth ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, float inWidth ) {
     	(void) HIViewErrorsRegistrationDependency();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentContentWidth ( inSegmentedView, inSegmentIndexOneBased, false, inWidth ));
		}

	struct HISegmentedViewGetSegmentContentWidth_Result {
		Boolean isAutoCalculated;
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

#if 0
	template <typename returnType>
	returnType HISegmentedViewCopySegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased );
	
	template < Nucleus::Owned<CGImageRef> > HISegmentedViewCopySegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased );
//	template < Nucleus::Owned<IconRef> >    HISegmentedViewCopySegmentImage;
#endif
	 }
	
#endif /* NITROGEN_HIVIEW_H */

