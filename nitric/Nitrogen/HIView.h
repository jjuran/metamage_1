#ifndef NITROGEN_HIVIEW_H
#define NITROGEN_HIVIEW_H

#if !TARGET_RT_MAC_MACHO
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

	void RegisterHIViewErrors();

/*==============================================================================*/
/*  EMBEDDING                                                                   */
/*==============================================================================*/

/*	extern HIViewRef HIViewGetRoot(WindowRef inWindow) */
	using ::HIViewGetRoot;

	inline void HIViewAddSubview ( HIViewRef inParent, HIViewRef inNewChild ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewAddSubview ( inParent, inNewChild ));
		}
	
	inline void HIViewRemoveFromSuperview ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
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
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewSetVisible ( inView, inVisible ));
		}
	
/*	extern Boolean HIViewIsVisible(HIViewRef inView) */
	using ::HIViewIsVisible;


/*==============================================================================*/
/*  POSITIONING                                                                 */
/*==============================================================================*/

	inline HIRect HIViewGetBounds ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIViewGetBounds ( inView, &retVal ));
		return retVal;
		}
	
	inline HIRect HIViewGetFrame ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIViewGetFrame ( inView, &retVal ));
		return retVal;
		}
	
	inline void HIViewSetFrame ( HIViewRef inView, const HIRect &inRect ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewSetFrame ( inView, &inRect ));
		}
	
	inline void HIViewMoveBy ( HIViewRef inView, float inDX, float inDY ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewMoveBy ( inView, inDX, inDY ));
		}

	inline void HIViewPlaceInSuperviewAt ( HIViewRef inView, float inX, float inY ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewPlaceInSuperviewAt ( inView, inX, inY ));
		}

	inline void HIViewReshapeStructure ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewReshapeStructure ( inView ));
		}

	inline void HIViewRegionChanged ( HIViewRef inView, ControlPartCode inRegionCode ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewRegionChanged ( inView, inRegionCode ));
		}

	
/*==============================================================================*/
/*  HIT TESTING/EVENT HANDLING                                                  */
/*==============================================================================*/

	inline HIViewRef HIViewGetViewForMouseEvent ( HIViewRef inView, EventRef inEvent ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIViewGetViewForMouseEvent ( inView, inEvent, &retVal ));
		return retVal;
		}

	inline void HIViewClick (HIViewRef inView, EventRef inEvent ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewClick ( inView, inEvent ));
		}
	
	inline ControlPartCode HIViewSimulateClick ( HIViewRef inView, 
				HIViewPartCode inPartToClick, UInt32 inModifiers ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		::ControlPartCode retVal;
		ThrowOSStatus ( ::HIViewSimulateClick ( inView, inPartToClick, inModifiers, &retVal ));
		return retVal;
		}
	
	inline HIViewPartCode HIViewGetPartHit ( HIViewRef inView, const HIPoint &inPoint ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewPartCode retVal;
		ThrowOSStatus ( ::HIViewGetPartHit ( inView, &inPoint, &retVal ));
		return retVal;
		}
	
	inline HIViewRef HIViewGetSubviewHit ( HIViewRef inView, 
							const HIPoint &inPoint, Boolean inDeep ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
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
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewSetNeedsDisplay ( inView, inNeedsDisplay ));
		}
	
	inline void HIViewSetNeedsDisplayInRegion ( HIViewRef inView, RgnHandle inRgn, Boolean inNeedsDisplay ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewSetNeedsDisplayInRegion ( inView, inRgn, inNeedsDisplay ));
		}
	
	inline void HIViewRender ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewRender ( inView ));
		}

	inline void HIViewFlashDirtyArea ( WindowRef inWindow ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewFlashDirtyArea ( inWindow ));
		}

	struct HIViewGetSizeConstraints_Result {
		HISize	minSize;
		HISize	maxSize;
		};
	
	inline HIViewGetSizeConstraints_Result HIViewGetSizeConstraints ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewGetSizeConstraints_Result retVal;
		ThrowOSStatus ( ::HIViewGetSizeConstraints ( inView, &retVal.minSize, &retVal.maxSize ));
		return retVal;
		}


/*==============================================================================*/
/*  COORDINATE SYSTEM CONVERSION                                                */
/*==============================================================================*/

	inline void HIViewConvertPoint ( HIPoint &ioPoint, HIViewRef inSourceView, HIViewRef inDestView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewConvertPoint ( &ioPoint, inSourceView, inDestView ));
		}
	
	inline void HIViewConvertRect ( HIRect &ioRect, HIViewRef inSourceView, HIViewRef inDestView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewConvertRect ( &ioRect, inSourceView, inDestView ));
		}

	inline void HIViewConvertRegion ( RgnHandle ioRgn, HIViewRef inSourceView, HIViewRef inDestView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewConvertRegion ( ioRgn, inSourceView, inDestView ));
		}

#if 0
//	Returns a new, converted region. Original region is untouched
	inline Nucleus::Owned<RgnHandle> HIViewConvertRegion ( RgnHandle inRgn, HIViewRef inSourceView, HIViewRef inDestView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewConvertRegion ( ioRgn, inSourceView, inDestView ));
		}
#endif

	inline void HIViewSetDrawingEnabled ( HIViewRef inView, Boolean inEnabled ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewSetDrawingEnabled ( inView, inEnabled ));
		}

/*	extern Boolean HIViewIsDrawingEnabled(HIViewRef inView) */
	using ::HIViewSetDrawingEnabled;

	inline void HIViewScrollRect ( HIViewRef inView, const HIRect &inRect, float inDX, float inDY ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewScrollRect ( inView, &inRect, inDX, inDY ));
		}
		
//	Scroll the whole view
	inline void HIViewScrollRect ( HIViewRef inView, float inDX, float inDY ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewScrollRect ( inView, NULL, inDX, inDY ));
		}

	inline void HIViewSetBoundsOrigin ( HIViewRef inView, float inX, float inY ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewSetBoundsOrigin ( inView, inX, inY ));
		}


/*==============================================================================*/
/*  KEYBOARD FOCUS                                                              */
/*==============================================================================*/

	inline void HIViewAdvanceFocus ( HIViewRef inRootForFocus, EventModifiers inModifiers ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewAdvanceFocus ( inRootForFocus, inModifiers ));
		}
	
	inline HIViewPartCode HIViewGetFocusPart ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewPartCode retVal;
		ThrowOSStatus ( ::HIViewGetFocusPart ( inView, &retVal ));
		return retVal;
		}

/*	extern Boolean HIViewSubtreeContainsFocus(HIViewRef inSubtreeStart) */
	using ::HIViewSubtreeContainsFocus;

	inline void HIViewSetNextFocus ( HIViewRef inView, HIViewRef inNextFocus ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewSetNextFocus ( inView, inNextFocus ));
		}
	
	inline void HIViewSetFirstSubViewFocus ( HIViewRef inParent, HIViewRef inSubView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewSetFirstSubViewFocus ( inParent, inSubView ));
		}
	

/*==============================================================================*/
/*  LAYOUT                                                                      */
/*==============================================================================*/

	inline HILayoutInfo HIViewGetLayoutInfo ( HIViewRef inView, UInt32 version = kHILayoutInfoVersionZero ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HILayoutInfo retVal;
		retVal.version = version;
		ThrowOSStatus ( ::HIViewGetLayoutInfo ( inView, &retVal ));
		return retVal;
		}

	inline void HIViewSetLayoutInfo ( HIViewRef inView, const HILayoutInfo &inLayoutInfo ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewSetLayoutInfo ( inView, &inLayoutInfo ));
		}
			
	inline void HIViewSuspendLayout ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewSuspendLayout ( inView ));
		}
	
	inline void HIViewResumeLayout ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewResumeLayout ( inView ));
		}

/*	extern Boolean HIViewIsLayoutActive(HIViewRef inView) */
	using ::HIViewIsLayoutActive;
	
	inline void HIViewApplyLayout ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewApplyLayout ( inView ));
		}

/*==============================================================================*/
/*  MISCELLANEOUS                                                               */
/*==============================================================================*/

/*	extern WindowRef HIViewGetWindow(HIViewRef inView) */
	using ::HIViewGetWindow;

	inline HIViewRef HIViewFindByID ( HIViewRef inStartView, HIViewID inID ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIViewFindByID ( inStartView, inID, &retVal ));
		return retVal;
		}

	inline OptionBits HIViewGetAttributes ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		::OptionBits retVal;
		ThrowOSStatus ( ::HIViewGetAttributes ( inView, &retVal ));
		return retVal;
		}

	inline void HIViewChangeAttributes ( HIViewRef inView, OptionBits inAttrsToSet, OptionBits inAttrsToClear ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewChangeAttributes ( inView, inAttrsToSet, inAttrsToClear ));
		}

	struct HIViewCreateOffscreenImage_Result {
		HIRect frame;
 		Nucleus::Owned<CGImageRef> image;
		operator Nucleus::Owned<CGImageRef> () { return image; } /* non-const, returns owned */
		operator CGImageRef () const  { return image; }
 		};
	
	HIViewCreateOffscreenImage_Result HIViewCreateOffscreenImage ( HIViewRef inView, OptionBits inOptions = 0 );

	inline void HIViewDrawCGImage ( CGContextRef inContext, const HIRect &inBounds, CGImageRef inImage ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewDrawCGImage ( inContext, &inBounds, inImage ));
		}
	
	inline HIViewFeatures HIViewGetFeatures ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewFeatures retVal;
		ThrowOSStatus ( ::HIViewGetFeatures ( inView, &retVal ));
		return retVal;
		}

	inline void HIViewChangeFeatures ( HIViewRef inView, HIViewFeatures inFeaturesToSet, HIViewFeatures inFeaturesToClear ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIViewChangeFeatures ( inView, inFeaturesToSet, inFeaturesToClear ));
		}


/*==============================================================================*/
/* HIGrowBoxView                                                                */
/*==============================================================================*/

	inline void HIGrowBoxViewSetTransparent ( HIViewRef inGrowBoxView, Boolean inTransparent ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIGrowBoxViewSetTransparent ( inGrowBoxView, inTransparent ));
		}

/*	extern Boolean HIGrowBoxViewIsTransparent(HIViewRef inGrowBoxView) */
	using ::HIGrowBoxViewIsTransparent;
	

/*==============================================================================*/
/*  HIScrollView                                                                */
/*==============================================================================*/

	inline Nucleus::Owned<HIViewRef> HIScrollViewCreate ( OptionBits inOptions ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIScrollViewCreate ( inOptions, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline void HIScrollViewSetScrollBarAutoHide ( HIViewRef inGrowBoxView, Boolean inAutoHide ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIScrollViewSetScrollBarAutoHide ( inGrowBoxView, inAutoHide ));
		}

/*	extern Boolean HIScrollViewGetScrollBarAutoHide(HIViewRef inView) */
	using ::HIScrollViewGetScrollBarAutoHide;
	
	inline void HIScrollViewNavigate ( HIViewRef inGrowBoxView, HIScrollViewAction inAction ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIScrollViewNavigate ( inGrowBoxView, inAction ));
		}

/*	extern Boolean HIScrollViewCanNavigate(HIViewRef inView, HIScrollViewAction inAction) */
	using ::HIScrollViewCanNavigate;
	

/*==============================================================================*/
/*  HIImageView                                                                 */
/*==============================================================================*/

//	I think that this is a bug in the headers. This should return an HIViewRef, not a ControlRef
	inline Nucleus::Owned<HIViewRef> HIImageViewCreate ( CGImageRef inImage ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIImageViewCreate ( inImage, &retVal ));
		return Nucleus::Owned<ControlRef>::Seize ( retVal );
		}

	inline void HIImageViewSetOpaque ( HIViewRef inView, Boolean inOpaque ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIImageViewSetOpaque ( inView, inOpaque ));
		}

/*	extern Boolean HIImageViewIsOpaque(HIViewRef inView) */
	using ::HIImageViewSetOpaque;
	
	inline void HIImageViewSetAlpha ( HIViewRef inView, float inAlpha ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIImageViewSetAlpha ( inView, inAlpha ));
		}

/*	extern float HIImageViewGetAlpha(HIViewRef inView) */
	using ::HIImageViewGetAlpha;

	inline void HIImageViewSetScaleToFit ( HIViewRef inView, Boolean inScaleToFit ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIImageViewSetScaleToFit ( inView, inScaleToFit ));
		}

/*	extern Boolean HIImageViewGetScaleToFit(HIViewRef inView) */
	using ::HIImageViewGetScaleToFit;


	inline void HIImageViewSetImage ( HIViewRef inView, CGImageRef inImage ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIImageViewSetImage ( inView, inImage ));
		}

	inline Nucleus::Owned<CGImageRef> HIImageViewCopyImage ( HIViewRef inView ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		return Nucleus::Owned<CGImageRef>::Seize ( ::HIImageViewCopyImage ( inView ));
		}


/*==============================================================================*/
/* HIComboBox                                                                   */
/*==============================================================================*/

//	Several variations
	inline Nucleus::Owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, OptionBits inAttributes ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, NULL, NULL, NULL, inAttributes, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}
	
	inline Nucleus::Owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, CFArrayRef list, OptionBits inAttributes ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, NULL, NULL, list, inAttributes, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline Nucleus::Owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, CFStringRef text, CFArrayRef list, OptionBits inAttributes ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, text, NULL, list, inAttributes, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline Nucleus::Owned<HIViewRef> HIComboBoxCreate ( const HIRect &boundsRect, CFStringRef text, 
							const ControlFontStyleRec &style, CFArrayRef list, OptionBits inAttributes ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIComboBoxCreate ( &boundsRect, text, &style, list, inAttributes, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

/*	extern ItemCount HIComboBoxGetItemCount(HIViewRef inComboBox) */
	using ::HIComboBoxGetItemCount;
	
	inline void HIComboBoxInsertTextItemAtIndex ( HIViewRef inComboBox, CFIndex inIndex, CFStringRef inText ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIComboBoxInsertTextItemAtIndex ( inComboBox, inIndex, inText ));
		}

	inline CFIndex HIComboBoxAppendTextItem ( HIViewRef inComboBox, CFStringRef inText ) {
		CFIndex retVal;
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIComboBoxAppendTextItem ( inComboBox, inText, &retVal ));
		return retVal;
		}

	inline Nucleus::Owned<CFStringRef> HIComboBoxCopyTextItemAtIndex ( HIViewRef inComboBox, CFIndex inIndex ) {
		CFStringRef retVal;
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIComboBoxCopyTextItemAtIndex ( inComboBox, inIndex, &retVal ));
		return Nucleus::Owned<CFStringRef>::Seize ( retVal );
		}

	inline void HIComboBoxRemoveItemAtIndex ( HIViewRef inComboBox, CFIndex inIndex ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIComboBoxRemoveItemAtIndex ( inComboBox, inIndex ));
		}

	inline void HIComboBoxChangeAttributes ( HIViewRef inComboBox, OptionBits inAttributesToSet, OptionBits inAttributesToClear ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HIComboBoxChangeAttributes ( inComboBox, inAttributesToSet, inAttributesToClear ));
		}

	inline OptionBits HIComboBoxGetAttributes ( HIViewRef inComboBox ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		::OptionBits retVal;
		ThrowOSStatus ( ::HIComboBoxGetAttributes ( inComboBox, &retVal ));
		return retVal;
		}


/*==============================================================================*/
/*  HISearchField                                                               */
/*==============================================================================*/

	inline Nucleus::Owned<HIViewRef> HISearchFieldCreate ( OptionBits inAttributes ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HISearchFieldCreate ( NULL, inAttributes, NULL, NULL, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}
	
	inline Nucleus::Owned<HIViewRef> HISearchFieldCreate ( const HIRect &inBounds, OptionBits inAttributes ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HISearchFieldCreate ( &inBounds, inAttributes, NULL, NULL, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline Nucleus::Owned<HIViewRef> HISearchFieldCreate ( const HIRect &inBounds, OptionBits inAttributes,
							MenuRef inSearchMenu, CFStringRef inDescriptiveText ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HISearchFieldCreate ( &inBounds, inAttributes, inSearchMenu, inDescriptiveText, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline void HISearchFieldSetSearchMenu ( HIViewRef inSearchField, MenuRef inSearchMenu ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISearchFieldSetSearchMenu ( inSearchField, inSearchMenu ));
		}

	inline MenuRef HISearchFieldGetSearchMenu ( HIViewRef inSearchField ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		MenuRef retVal;
		ThrowOSStatus ( ::HISearchFieldGetSearchMenu ( inSearchField, &retVal ));
		return retVal;
		}

	inline void HISearchFieldChangeAttributes ( HIViewRef inSearchField, OptionBits inAttributesToSet, OptionBits inAttributesToClear ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISearchFieldChangeAttributes ( inSearchField, inAttributesToSet, inAttributesToClear ));
		}

	inline OptionBits HISearchFieldGetAttributes ( HIViewRef inSearchField ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		::OptionBits retVal;
		ThrowOSStatus ( ::HISearchFieldGetAttributes ( inSearchField, &retVal ));
		return retVal;
		}

	inline void HISearchFieldSetDescriptiveText ( HIViewRef inSearchField, CFStringRef inDescription ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISearchFieldSetDescriptiveText ( inSearchField, inDescription ));
		}

	inline Nucleus::Owned<CFStringRef> HISearchFieldCopyDescriptiveText ( HIViewRef inSearchField ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
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
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HIMenuGetContentView ( inMenu, inMenuType, &retVal ));
		return retVal;
		}


/*==============================================================================*/
/*  HISegmentedView                                                             */
/*==============================================================================*/

	inline Nucleus::Owned<HIViewRef> HISegmentedViewCreate ( const HIRect &inBounds ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		HIViewRef retVal;
		ThrowOSStatus ( ::HISegmentedViewCreate ( &inBounds, &retVal ));
		return Nucleus::Owned<HIViewRef>::Seize ( retVal );
		}

	inline void HISegmentedViewSetSegmentCount ( HIViewRef inSegmentedView, UInt32 inSegmentCount ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentCount ( inSegmentedView, inSegmentCount ));
		}

/*	extern UInt32 HISegmentedViewGetSegmentCount(HIViewRef inSegmentedView) */
	using ::HISegmentedViewGetSegmentCount;

	inline void HISegmentedViewSetSegmentBehavior ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, 
													HISegmentBehavior inBehavior ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentBehavior ( inSegmentedView, inSegmentIndexOneBased, inBehavior ));
		}

/*	extern HISegmentBehavior HISegmentedViewGetSegmentBehavior(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentBehavior;

	inline void HISegmentedViewChangeSegmentAttributes ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, 
					OptionBits inAttributesToSet, OptionBits inAttributesToClear ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISegmentedViewChangeSegmentAttributes ( inSegmentedView, inSegmentIndexOneBased, inAttributesToSet, inAttributesToClear ));
		}

/*	extern OptionBits HISegmentedViewGetSegmentAttributes(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentAttributes;

	inline void HISegmentedViewSetSegmentValue ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, SInt32 inValue ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentValue ( inSegmentedView, inSegmentIndexOneBased, inValue ));
		}

/*	extern HISegmentBehavior HISegmentedViewGetSegmentValue(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentValue;

	inline void HISegmentedViewSetSegmentEnabled ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, Boolean inEnabled ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentEnabled ( inSegmentedView, inSegmentIndexOneBased, inEnabled ));
		}

/*	extern HISegmentBehavior HISegmentedViewIsSegmentEnabled(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewIsSegmentEnabled;

	inline void HISegmentedViewSetSegmentCommand ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, UInt32 inCommand ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentCommand ( inSegmentedView, inSegmentIndexOneBased, inCommand ));
		}

/*	extern HISegmentBehavior HISegmentedViewGetSegmentCommand(HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased) */
	using ::HISegmentedViewGetSegmentCommand;

	inline void HISegmentedViewSetSegmentLabel ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, CFStringRef inLabel ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentLabel ( inSegmentedView, inSegmentIndexOneBased, inLabel ));
		}

	inline Nucleus::Owned<CFStringRef> HISegmentedViewCopySegmentLabel ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		CFStringRef retVal;
		ThrowOSStatus ( ::HISegmentedViewCopySegmentLabel ( inSegmentedView, inSegmentIndexOneBased, &retVal ));
		return Nucleus::Owned<CFStringRef>::Seize ( retVal );
		}

//	Two versions here, one for "auto-width", the other for "explicit width".
	inline void HISegmentedViewSetSegmentContentWidth ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, Boolean inAutoCalculateWidth ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentContentWidth ( inSegmentedView, inSegmentIndexOneBased, inAutoCalculateWidth, 0.0 ));
		}

	inline void HISegmentedViewSetSegmentContentWidth ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, float inWidth ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
		ThrowOSStatus ( ::HISegmentedViewSetSegmentContentWidth ( inSegmentedView, inSegmentIndexOneBased, false, inWidth ));
		}

	struct HISegmentedViewGetSegmentContentWidth_Result {
		Boolean isAutoCalculated;
		float	segmentWidth;
		operator float () const { return segmentWidth; }
		};
		
	inline HISegmentedViewGetSegmentContentWidth_Result HISegmentedViewGetSegmentContentWidth ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased ) {
     	Nucleus::OnlyOnce<RegisterHIViewErrors>();
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

