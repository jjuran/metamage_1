#ifndef NITROGEN_HITHENE_H
#define	NITROGEN_HITHENE_H

#ifndef	__MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __HITHEME__
#include <HIToolbox/HITheme.h>
#endif

#ifndef	NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen {
	void RegisterHIThemeErrors ();
	
/* -------------------------------------------------------------------------- */
/*  Buttons                                                                   */
/* -------------------------------------------------------------------------- */

	inline HIRect HIThemeDrawButton ( const HIRect &inBounds, const HIThemeButtonDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeDrawButton ( &inBounds, &inDrawInfo, inContext, inOrientation, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetButtonShape ( const HIRect &inBounds, const HIThemeButtonDrawInfo &inDrawInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIShapeRef retVal;
		ThrowOSStatus ( ::HIThemeGetButtonShape ( &inBounds, &inDrawInfo, &retVal ));
		return retVal;
		}

	inline HIRect HIThemeGetButtonContentBounds ( const HIRect &inBounds, const HIThemeButtonDrawInfo &inDrawInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeGetButtonContentBounds ( &inBounds, &inDrawInfo, &retVal ));
		return retVal;
		}

	inline HIRect HIThemeGetButtonBackgroundBounds ( const HIRect &inBounds, const HIThemeButtonDrawInfo &inDrawInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeGetButtonBackgroundBounds ( &inBounds, &inDrawInfo, &retVal ));
		return retVal;
		}

	inline void HIThemeDrawChasingArrows ( const HIRect &inBounds, const HIThemeChasingArrowsDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawChasingArrows ( &inBounds, &inDrawInfo, inContext, inOrientation ));
		}
	
	inline void HIThemeDrawPopupArrow ( const HIRect &inBounds, const HIThemePopupArrowDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawPopupArrow ( &inBounds, &inDrawInfo, inContext, inOrientation ));
		}
	

/* -------------------------------------------------------------------------- */
/*  Menus                                                                     */
/* -------------------------------------------------------------------------- */

	inline void HIThemeDrawMenuBarBackground ( const HIRect &inBounds, const HIThemeMenuBarDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawMenuBarBackground ( &inBounds, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIRect HIThemeDrawMenuTitle ( const HIRect &inMenuBarRect, const HIRect &inTitleRect, 
					const HIThemeMenuTitleDrawInfo &inDrawInfo, CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeDrawMenuTitle ( &inMenuBarRect, &inTitleRect, &inDrawInfo, inContext, inOrientation, &retVal ));
		return retVal;
		}

	inline void HIThemeDrawMenuBackground ( const HIRect &inMenuRect, const HIThemeMenuDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawMenuBackground ( &inMenuRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIRect HIThemeDrawMenuItem ( const HIRect &inMenuRect, const HIRect &inItemRect, 
					const HIThemeMenuItemDrawInfo &inDrawInfo, CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeDrawMenuItem ( &inMenuRect, &inItemRect, &inDrawInfo, inContext, inOrientation, &retVal ));
		return retVal;
		}

	inline void HIThemeDrawMenuSeparator ( const HIRect &inMenuRect, const HIRect &inItemRect, 
					const HIThemeMenuItemDrawInfo &inDrawInfo, CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawMenuSeparator ( &inMenuRect, &inItemRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIShapeRef HIThemeGetMenuBackgroundShape ( const HIRect &inMenuRect, const HIThemeMenuDrawInfo &inDrawInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIShapeRef retVal;
		ThrowOSStatus ( ::HIThemeGetMenuBackgroundShape ( &inMenuRect, &inDrawInfo, &retVal ));
		return retVal;
		}


/* -------------------------------------------------------------------------- */
/*  Tabs                                                                      */
/* -------------------------------------------------------------------------- */

	inline void HIThemeDrawTabPane ( const HIRect &inRect, const HIThemeTabPaneDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawTabPane ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIRect HIThemeDrawTab ( const HIRect &inRect, 
					const HIThemeTabDrawInfo &inDrawInfo, CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeDrawTab ( &inRect, &inDrawInfo, inContext, inOrientation, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetTabPaneDrawShape ( const HIRect &inRect, ThemeTabDirection inDirection, HIThemeTabSize inTabSize ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIShapeRef retVal;
		ThrowOSStatus ( ::HIThemeGetTabPaneDrawShape ( &inRect, inDirection, inTabSize, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetTabPaneContentShape ( const HIRect &inRect, ThemeTabDirection inDirection, HIThemeTabSize inTabSize ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIShapeRef retVal;
		ThrowOSStatus ( ::HIThemeGetTabPaneContentShape ( &inRect, inDirection, inTabSize, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetTabDrawShape ( const HIRect &inRect, const HIThemeTabDrawInfo &inDrawInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIShapeRef retVal;
		ThrowOSStatus ( ::HIThemeGetTabDrawShape ( &inRect, &inDrawInfo, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetTabShape ( const HIRect &inRect, const HIThemeTabDrawInfo &inDrawInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIShapeRef retVal;
		ThrowOSStatus ( ::HIThemeGetTabShape ( &inRect, &inDrawInfo, &retVal ));
		return retVal;
		}


/* -------------------------------------------------------------------------- */
/*  Text                                                                      */
/* -------------------------------------------------------------------------- */

	struct HIThemeGetTextDimensions_Result {
		float textWidth;
		float textHeight;
		float textBaseline;
		operator float () const  { return textWidth; }
		};
	
//	!!! Marshall says - what an awful API
	inline HIThemeGetTextDimensions_Result HIThemeGetTextDimensions ( CFStringRef inString,
  							float inWidth, HIThemeTextInfo &inTextInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
  		HIThemeGetTextDimensions_Result retVal;
  		ThrowOSStatus ( ::HIThemeGetTextDimensions ( inString, inWidth, &inTextInfo,
  							&retVal.textWidth, &retVal.textHeight, &retVal.textBaseline ));
  		return retVal;
  		}
  	
	inline void HIThemeDrawTextBox ( CFStringRef inString, const HIRect &inBounds, HIThemeTextInfo &inTextInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawTextBox ( inString, &inBounds, &inTextInfo, inContext, inOrientation ));
		}
	

/* -------------------------------------------------------------------------- */
/*  Tracks                                                                    */
/* -------------------------------------------------------------------------- */

	inline void HIThemeDrawTrack ( const HIThemeTrackDrawInfo &inDrawInfo, const HIRect &inBounds,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawTrack ( &inDrawInfo, &inBounds, inContext, inOrientation ));
		}
	
	inline void HIThemeDrawTrackTickMarks ( const HIThemeTrackDrawInfo &inDrawInfo, ItemCount inNumTicks,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawTrackTickMarks ( &inDrawInfo, inNumTicks, inContext, inOrientation ));
		}

	inline void HIThemeDrawTickMark ( const HIRect &inBounds, const HIThemeTickMarkDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawTickMark ( &inBounds, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIShapeRef HIThemeGetTrackThumbShape ( const HIThemeTrackDrawInfo &inDrawInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIShapeRef retVal;
		ThrowOSStatus ( ::HIThemeGetTrackThumbShape ( &inDrawInfo, &retVal ));
		return retVal;
		}

	struct HIThemeHitTestTrack_Result {
		Boolean trackWasHit;
		ControlPartCode trackPartHit;
		operator Boolean () const  { return trackWasHit; }
		};

//	!!! This is awkward
	inline HIThemeHitTestTrack_Result HIThemeHitTestTrack ( const HIThemeTrackDrawInfo &inDrawInfo, const HIPoint &inMousePoint ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIThemeHitTestTrack_Result retVal;
		retVal.trackWasHit = ::HIThemeHitTestTrack ( &inDrawInfo, &inMousePoint, &retVal.trackPartHit );
		return retVal;
		}

	inline HIRect HIThemeGetTrackBounds ( const HIThemeTrackDrawInfo &inDrawInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeGetTrackBounds ( &inDrawInfo, &retVal ));
		return retVal;
		}

	inline HIRect HIThemeGetTrackPartBounds ( const HIThemeTrackDrawInfo &inDrawInfo, ControlPartCode inPartCode ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeGetTrackPartBounds ( &inDrawInfo, inPartCode, &retVal ));
		return retVal;
		}

//	A helper function for HIThemeGetTrackParts
	inline UInt32 HIThemeCountTrackParts ( const HIThemeTrackDrawInfo &inDrawInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		UInt32 retVal;
		ThrowOSStatus ( ::HIThemeGetTrackParts ( &inDrawInfo, &retVal, 0, NULL ));
		return retVal;
		}

	inline UInt32 HIThemeGetTrackParts ( const HIThemeTrackDrawInfo &inDrawInfo, 
								UInt32 ioMaxParts, ControlPartCode *ioPartsBuffer ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		UInt32 retVal;
		ThrowOSStatus ( ::HIThemeGetTrackParts ( &inDrawInfo, &retVal, ioMaxParts, ioPartsBuffer ));
		return retVal;
		}
	
	inline HIRect HIThemeGetTrackDragRect ( const HIThemeTrackDrawInfo &inDrawInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeGetTrackDragRect ( &inDrawInfo, &retVal ));
		return retVal;
		}

	inline float HIThemeGetTrackThumbPositionFromOffset ( const HIThemeTrackDrawInfo &inDrawInfo, const HIPoint &inThumbOffset ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		float retVal;
		ThrowOSStatus ( ::HIThemeGetTrackThumbPositionFromOffset ( &inDrawInfo, &inThumbOffset, &retVal ));
		return retVal;
		}

	inline float HIThemeGetTrackThumbPositionFromBounds ( const HIThemeTrackDrawInfo &inDrawInfo, const HIRect &inThumbBounds ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		float retVal;
		ThrowOSStatus ( ::HIThemeGetTrackThumbPositionFromBounds ( &inDrawInfo, &inThumbBounds, &retVal ));
		return retVal;
		}

	inline SInt32 HIThemeGetTrackLiveValue ( const HIThemeTrackDrawInfo &inDrawInfo, float inRelativePosition ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		SInt32 retVal;
		ThrowOSStatus ( ::HIThemeGetTrackLiveValue ( &inDrawInfo, inRelativePosition, &retVal ));
		return retVal;
		}

	inline HIRect HIThemeGetScrollBarTrackRect ( const HIRect &inBounds, const HIScrollBarTrackInfo &inTrackInfo, Boolean inIsHoriz ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeGetScrollBarTrackRect ( &inBounds, &inTrackInfo, inIsHoriz, &retVal ));
		return retVal;
		}

	struct HIThemeHitTestScrollBarArrows_Result {
		Boolean			controlWasHit;
		HIRect 			trackBounds;
		ControlPartCode	partCode;
		operator Boolean () const  { return controlWasHit; }
		};
	
	inline HIThemeHitTestScrollBarArrows_Result HIThemeHitTestScrollBarArrows ( const HIRect &inBounds, 
					const HIScrollBarTrackInfo &inTrackInfo, Boolean inIsHoriz, const HIPoint &inPtHit ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIThemeHitTestScrollBarArrows_Result retVal;
		retVal.controlWasHit = ::HIThemeHitTestScrollBarArrows ( &inBounds, &inTrackInfo, inIsHoriz, &inPtHit,
											&retVal.trackBounds, &retVal.partCode );
		return retVal;
		}
	
	inline void HIThemeDrawScrollBarDelimiters ( const HIRect &inContRect, const HIThemeScrollBarDelimitersDrawInfo &inDrawInfo, 
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawScrollBarDelimiters ( &inContRect, &inDrawInfo, inContext, inOrientation ));
		}
	

/* -------------------------------------------------------------------------- */
/*  Windows                                                                   */
/* -------------------------------------------------------------------------- */

	inline HIRect HIThemeDrawWindowFrame ( const HIRect &inContRect, const HIThemeWindowDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeDrawWindowFrame ( &inContRect, &inDrawInfo, inContext, inOrientation, &retVal ));
		return retVal;
		}

	inline void HIThemeDrawTitleBarWidget ( const HIRect &inContRect, const HIThemeWindowWidgetDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawTitleBarWidget ( &inContRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawGrowBox ( const HIPoint &inOrigin, const HIThemeGrowBoxDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawGrowBox ( &inOrigin, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIRect HIThemeGetGrowBoxBounds ( const HIPoint &inOrigin, const HIThemeGrowBoxDrawInfo &inTrackInfo ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIRect retVal;
		ThrowOSStatus ( ::HIThemeGetGrowBoxBounds ( &inOrigin, &inTrackInfo, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetWindowShape ( const HIRect &inContRect, const HIThemeWindowDrawInfo &inTrackInfo,
												WindowRegionCode inWinRegion ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		HIShapeRef retVal;
		ThrowOSStatus ( ::HIThemeGetWindowShape ( &inContRect, &inTrackInfo, inWinRegion, &retVal ));
		return retVal;
		}

	inline Boolean HIThemeGetWindowRegionHit ( const HIRect &inContRect, const HIThemeWindowDrawInfo &inTrackInfo,
												const HIPoint &inPoint, WindowRegionCode &outRegionHit ) {
		return ::HIThemeGetWindowRegionHit ( &inContRect, &inTrackInfo, &inPoint, &outRegionHit );
		}
	

/* -------------------------------------------------------------------------- */
/*  Miscellaneous                                                             */
/* -------------------------------------------------------------------------- */

	inline void HIThemeDrawFrame ( const HIRect &inRect, const HIThemeFrameDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawFrame ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawGroupBox ( const HIRect &inRect, const HIThemeGroupBoxDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawGroupBox ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawGenericWell ( const HIRect &inRect, const HIThemeButtonDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawGenericWell ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawPaneSplitter ( const HIRect &inRect, const HIThemeSplitterDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawPaneSplitter ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawGrabber ( const HIRect &inRect, const HIThemeGrabberDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawGrabber ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawPlacard ( const HIRect &inRect, const HIThemePlacardDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawPlacard ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawHeader ( const HIRect &inRect, const HIThemeHeaderDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawHeader ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawFocusRect ( const HIRect &inRect, Boolean inHasFocus,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawFocusRect ( &inRect, inHasFocus, inContext, inOrientation ));
		}

	inline void HIThemeDrawSeparator ( const HIRect &inRect, const HIThemeSeparatorDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawSeparator ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeApplyBackground ( const HIRect &inRect, const HIThemeBackgroundDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeApplyBackground ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawBackground ( const HIRect &inRect, const HIThemeBackgroundDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	OnlyOnce<RegisterHIThemeErrors>();
		ThrowOSStatus ( ::HIThemeDrawBackground ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	}

#endif /* __HITHEME__ */

