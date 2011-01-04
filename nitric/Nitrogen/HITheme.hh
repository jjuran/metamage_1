// Nitrogen/HITheme.hh
// -------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2007 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_HITHENE_HH
#define NITROGEN_HITHENE_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __HITHEME__
#include <Carbon/Carbon.h>
#endif

// Nitrogen
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif


namespace Nitrogen {
	
	class HIThemeErrorsRegistrationDependency
	{
		public:
			HIThemeErrorsRegistrationDependency();
	};
	
	inline HIThemeErrorsRegistrationDependency::HIThemeErrorsRegistrationDependency()
	{
		// Apple hasn't documented any HITheme errors.
	}
	
	
/* -------------------------------------------------------------------------- */
/*  Buttons                                                                   */
/* -------------------------------------------------------------------------- */

	inline HIRect HIThemeDrawButton ( const HIRect &inBounds, const HIThemeButtonDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeDrawButton ( &inBounds, &inDrawInfo, inContext, inOrientation, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetButtonShape ( const HIRect &inBounds, const HIThemeButtonDrawInfo &inDrawInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIShapeRef retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetButtonShape ( &inBounds, &inDrawInfo, &retVal ));
		return retVal;
		}

	inline HIRect HIThemeGetButtonContentBounds ( const HIRect &inBounds, const HIThemeButtonDrawInfo &inDrawInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetButtonContentBounds ( &inBounds, &inDrawInfo, &retVal ));
		return retVal;
		}

	inline HIRect HIThemeGetButtonBackgroundBounds ( const HIRect &inBounds, const HIThemeButtonDrawInfo &inDrawInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetButtonBackgroundBounds ( &inBounds, &inDrawInfo, &retVal ));
		return retVal;
		}

	inline void HIThemeDrawChasingArrows ( const HIRect &inBounds, const HIThemeChasingArrowsDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawChasingArrows ( &inBounds, &inDrawInfo, inContext, inOrientation ));
		}
	
	inline void HIThemeDrawPopupArrow ( const HIRect &inBounds, const HIThemePopupArrowDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawPopupArrow ( &inBounds, &inDrawInfo, inContext, inOrientation ));
		}
	

/* -------------------------------------------------------------------------- */
/*  Menus                                                                     */
/* -------------------------------------------------------------------------- */

	inline void HIThemeDrawMenuBarBackground ( const HIRect &inBounds, const HIThemeMenuBarDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawMenuBarBackground ( &inBounds, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIRect HIThemeDrawMenuTitle ( const HIRect &inMenuBarRect, const HIRect &inTitleRect, 
					const HIThemeMenuTitleDrawInfo &inDrawInfo, CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeDrawMenuTitle ( &inMenuBarRect, &inTitleRect, &inDrawInfo, inContext, inOrientation, &retVal ));
		return retVal;
		}

	inline void HIThemeDrawMenuBackground ( const HIRect &inMenuRect, const HIThemeMenuDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawMenuBackground ( &inMenuRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIRect HIThemeDrawMenuItem ( const HIRect &inMenuRect, const HIRect &inItemRect, 
					const HIThemeMenuItemDrawInfo &inDrawInfo, CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeDrawMenuItem ( &inMenuRect, &inItemRect, &inDrawInfo, inContext, inOrientation, &retVal ));
		return retVal;
		}

	inline void HIThemeDrawMenuSeparator ( const HIRect &inMenuRect, const HIRect &inItemRect, 
					const HIThemeMenuItemDrawInfo &inDrawInfo, CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawMenuSeparator ( &inMenuRect, &inItemRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIShapeRef HIThemeGetMenuBackgroundShape ( const HIRect &inMenuRect, const HIThemeMenuDrawInfo &inDrawInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIShapeRef retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetMenuBackgroundShape ( &inMenuRect, &inDrawInfo, &retVal ));
		return retVal;
		}


/* -------------------------------------------------------------------------- */
/*  Tabs                                                                      */
/* -------------------------------------------------------------------------- */

	inline void HIThemeDrawTabPane ( const HIRect &inRect, const HIThemeTabPaneDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawTabPane ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIRect HIThemeDrawTab ( const HIRect &inRect, 
					const HIThemeTabDrawInfo &inDrawInfo, CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeDrawTab ( &inRect, &inDrawInfo, inContext, inOrientation, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetTabPaneDrawShape ( const HIRect &inRect, ThemeTabDirection inDirection, HIThemeTabSize inTabSize ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIShapeRef retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTabPaneDrawShape ( &inRect, inDirection, inTabSize, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetTabPaneContentShape ( const HIRect &inRect, ThemeTabDirection inDirection, HIThemeTabSize inTabSize ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIShapeRef retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTabPaneContentShape ( &inRect, inDirection, inTabSize, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetTabDrawShape ( const HIRect &inRect, const HIThemeTabDrawInfo &inDrawInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIShapeRef retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTabDrawShape ( &inRect, &inDrawInfo, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetTabShape ( const HIRect &inRect, const HIThemeTabDrawInfo &inDrawInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIShapeRef retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTabShape ( &inRect, &inDrawInfo, &retVal ));
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
     	(void) HIThemeErrorsRegistrationDependency();
  		HIThemeGetTextDimensions_Result retVal;
  		Mac::ThrowOSStatus ( ::HIThemeGetTextDimensions ( inString, inWidth, &inTextInfo,
  							&retVal.textWidth, &retVal.textHeight, &retVal.textBaseline ));
  		return retVal;
  		}
  	
	inline void HIThemeDrawTextBox ( CFStringRef inString, const HIRect &inBounds, HIThemeTextInfo &inTextInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawTextBox ( inString, &inBounds, &inTextInfo, inContext, inOrientation ));
		}
	

/* -------------------------------------------------------------------------- */
/*  Tracks                                                                    */
/* -------------------------------------------------------------------------- */

	inline void HIThemeDrawTrack ( const HIThemeTrackDrawInfo &inDrawInfo, const HIRect &inBounds,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawTrack ( &inDrawInfo, &inBounds, inContext, inOrientation ));
		}
	
	inline void HIThemeDrawTrackTickMarks ( const HIThemeTrackDrawInfo &inDrawInfo, ItemCount inNumTicks,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawTrackTickMarks ( &inDrawInfo, inNumTicks, inContext, inOrientation ));
		}

	inline void HIThemeDrawTickMark ( const HIRect &inBounds, const HIThemeTickMarkDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawTickMark ( &inBounds, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIShapeRef HIThemeGetTrackThumbShape ( const HIThemeTrackDrawInfo &inDrawInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIShapeRef retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTrackThumbShape ( &inDrawInfo, &retVal ));
		return retVal;
		}

	struct HIThemeHitTestTrack_Result {
		bool trackWasHit;
		ControlPartCode trackPartHit;
		operator bool() const  { return trackWasHit; }
		};

//	!!! This is awkward
	inline HIThemeHitTestTrack_Result HIThemeHitTestTrack ( const HIThemeTrackDrawInfo &inDrawInfo, const HIPoint &inMousePoint ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIThemeHitTestTrack_Result retVal;
		::ControlPartCode trackPartHit;
		retVal.trackWasHit	= ::HIThemeHitTestTrack ( &inDrawInfo, &inMousePoint, &trackPartHit );
		retVal.trackPartHit = ControlPartCode( trackPartHit );
		return retVal;
		}

	inline HIRect HIThemeGetTrackBounds ( const HIThemeTrackDrawInfo &inDrawInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTrackBounds ( &inDrawInfo, &retVal ));
		return retVal;
		}

	inline HIRect HIThemeGetTrackPartBounds ( const HIThemeTrackDrawInfo &inDrawInfo, ControlPartCode inPartCode ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTrackPartBounds ( &inDrawInfo, inPartCode, &retVal ));
		return retVal;
		}

//	A helper function for HIThemeGetTrackParts
	inline UInt32 HIThemeCountTrackParts ( const HIThemeTrackDrawInfo &inDrawInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		UInt32 retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTrackParts ( &inDrawInfo, &retVal, 0, NULL ));
		return retVal;
		}

	inline UInt32 HIThemeGetTrackParts ( const HIThemeTrackDrawInfo &inDrawInfo, 
								UInt32 ioMaxParts, ::ControlPartCode *ioPartsBuffer ) {
     	(void) HIThemeErrorsRegistrationDependency();
		UInt32 retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTrackParts ( &inDrawInfo, &retVal, ioMaxParts, ioPartsBuffer ));
		return retVal;
		}
	
	inline HIRect HIThemeGetTrackDragRect ( const HIThemeTrackDrawInfo &inDrawInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTrackDragRect ( &inDrawInfo, &retVal ));
		return retVal;
		}

	inline float HIThemeGetTrackThumbPositionFromOffset ( const HIThemeTrackDrawInfo &inDrawInfo, const HIPoint &inThumbOffset ) {
     	(void) HIThemeErrorsRegistrationDependency();
		float retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTrackThumbPositionFromOffset ( &inDrawInfo, &inThumbOffset, &retVal ));
		return retVal;
		}

	inline float HIThemeGetTrackThumbPositionFromBounds ( const HIThemeTrackDrawInfo &inDrawInfo, const HIRect &inThumbBounds ) {
     	(void) HIThemeErrorsRegistrationDependency();
		float retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTrackThumbPositionFromBounds ( &inDrawInfo, &inThumbBounds, &retVal ));
		return retVal;
		}

	inline SInt32 HIThemeGetTrackLiveValue ( const HIThemeTrackDrawInfo &inDrawInfo, float inRelativePosition ) {
     	(void) HIThemeErrorsRegistrationDependency();
		SInt32 retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetTrackLiveValue ( &inDrawInfo, inRelativePosition, &retVal ));
		return retVal;
		}

	inline HIRect HIThemeGetScrollBarTrackRect ( const HIRect &inBounds, const HIScrollBarTrackInfo &inTrackInfo, bool inIsHoriz ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetScrollBarTrackRect ( &inBounds, &inTrackInfo, inIsHoriz, &retVal ));
		return retVal;
		}

	struct HIThemeHitTestScrollBarArrows_Result {
		bool			controlWasHit;
		HIRect 			trackBounds;
		ControlPartCode	partCode;
		operator bool() const  { return controlWasHit; }
		};
	
	inline HIThemeHitTestScrollBarArrows_Result HIThemeHitTestScrollBarArrows ( const HIRect &inBounds, 
					const HIScrollBarTrackInfo &inTrackInfo, bool inIsHoriz, const HIPoint &inPtHit ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIThemeHitTestScrollBarArrows_Result retVal;
		::ControlPartCode	partCode;
		retVal.controlWasHit = ::HIThemeHitTestScrollBarArrows ( &inBounds, &inTrackInfo, inIsHoriz, &inPtHit,
											&retVal.trackBounds, &partCode );
		retVal.partCode = ControlPartCode( partCode );
		return retVal;
		}
	
	inline void HIThemeDrawScrollBarDelimiters ( const HIRect &inContRect, const HIThemeScrollBarDelimitersDrawInfo &inDrawInfo, 
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawScrollBarDelimiters ( &inContRect, &inDrawInfo, inContext, inOrientation ));
		}
	

/* -------------------------------------------------------------------------- */
/*  Windows                                                                   */
/* -------------------------------------------------------------------------- */

	inline HIRect HIThemeDrawWindowFrame ( const HIRect &inContRect, const HIThemeWindowDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeDrawWindowFrame ( &inContRect, &inDrawInfo, inContext, inOrientation, &retVal ));
		return retVal;
		}

	inline void HIThemeDrawTitleBarWidget ( const HIRect &inContRect, const HIThemeWindowWidgetDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawTitleBarWidget ( &inContRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawGrowBox ( const HIPoint &inOrigin, const HIThemeGrowBoxDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawGrowBox ( &inOrigin, &inDrawInfo, inContext, inOrientation ));
		}

	inline HIRect HIThemeGetGrowBoxBounds ( const HIPoint &inOrigin, const HIThemeGrowBoxDrawInfo &inTrackInfo ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIRect retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetGrowBoxBounds ( &inOrigin, &inTrackInfo, &retVal ));
		return retVal;
		}

	inline HIShapeRef HIThemeGetWindowShape ( const HIRect &inContRect, const HIThemeWindowDrawInfo &inTrackInfo,
												WindowRegionCode inWinRegion ) {
     	(void) HIThemeErrorsRegistrationDependency();
		HIShapeRef retVal;
		Mac::ThrowOSStatus ( ::HIThemeGetWindowShape ( &inContRect, &inTrackInfo, inWinRegion, &retVal ));
		return retVal;
		}

   struct HIThemeGetWindowRegionHit_Result
     {
      bool               wasAnythingHit;
      ::WindowRegionCode regionHit;
     };
   
	inline HIThemeGetWindowRegionHit_Result HIThemeGetWindowRegionHit ( const HIRect &inContRect, 
				const HIThemeWindowDrawInfo &inTrackInfo, const HIPoint &inPoint ) {
		HIThemeGetWindowRegionHit_Result result;
		result.wasAnythingHit = ::HIThemeGetWindowRegionHit ( &inContRect, &inTrackInfo, &inPoint, &result.regionHit );
		return result;
		}
	

/* -------------------------------------------------------------------------- */
/*  Miscellaneous                                                             */
/* -------------------------------------------------------------------------- */

	inline void HIThemeDrawFrame ( const HIRect &inRect, const HIThemeFrameDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawFrame ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawGroupBox ( const HIRect &inRect, const HIThemeGroupBoxDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawGroupBox ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawGenericWell ( const HIRect &inRect, const HIThemeButtonDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawGenericWell ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawPaneSplitter ( const HIRect &inRect, const HIThemeSplitterDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawPaneSplitter ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawGrabber ( const HIRect &inRect, const HIThemeGrabberDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawGrabber ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawPlacard ( const HIRect &inRect, const HIThemePlacardDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawPlacard ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawHeader ( const HIRect &inRect, const HIThemeHeaderDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawHeader ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawFocusRect ( const HIRect &inRect, bool inHasFocus,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawFocusRect ( &inRect, inHasFocus, inContext, inOrientation ));
		}

	inline void HIThemeDrawSeparator ( const HIRect &inRect, const HIThemeSeparatorDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawSeparator ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeApplyBackground ( const HIRect &inRect, const HIThemeBackgroundDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeApplyBackground ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	inline void HIThemeDrawBackground ( const HIRect &inRect, const HIThemeBackgroundDrawInfo &inDrawInfo,
									  CGContextRef inContext, HIThemeOrientation inOrientation ) {
     	(void) HIThemeErrorsRegistrationDependency();
		Mac::ThrowOSStatus ( ::HIThemeDrawBackground ( &inRect, &inDrawInfo, inContext, inOrientation ));
		}

	}

#endif /* __HITHEME__ */

