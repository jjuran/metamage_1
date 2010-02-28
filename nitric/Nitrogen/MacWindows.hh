// Nitrogen/MacWindows.hh
// ----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2009 by Lisa Lippincott, Joshua Juran, and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_MACWINDOWS_HH
#define NITROGEN_MACWINDOWS_HH

#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"

#ifndef NUCLEUS_LINKEDLISTCONTAINER_H
#include "Nucleus/LinkedListContainer.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_ALIASES_HH
#include "Nitrogen/Aliases.hh"
#endif
#ifndef NITROGEN_APPLEEVENTS_HH
#include "Nitrogen/AppleEvents.hh"
#endif
#ifndef NITROGEN_COLLECTIONS_HH
#include "Nitrogen/Collections.hh"
#endif
#ifndef NITROGEN_DRAG_HH
#include "Nitrogen/Drag.hh"
#endif
#ifndef NITROGEN_EVENTS_HH
#include "Nitrogen/Events.hh"
#endif
#ifndef NITROGEN_MENUS_HH
#include "Nitrogen/Menus.hh"
#endif
#ifndef NITROGEN_MIXEDMODE_HH
#include "Nitrogen/MixedMode.hh"
#endif
#ifndef NITROGEN_QDOFFSCREEN_HH
#include "Nitrogen/QDOffscreen.hh"
#endif
#ifndef NITROGEN_QUICKDRAW_HH
#include "Nitrogen/Quickdraw.hh"
#endif
#ifndef NITROGEN_TEXTCOMMON_HH
#include "Nitrogen/TextCommon.hh"
#endif
#ifndef NITROGEN_ICONS_HH
#include "Nitrogen/Icons.hh"
#endif
#ifndef NITROGEN_CFSTRING_HH
#include "Nitrogen/CFString.hh"
#endif


#ifdef kFirstWindowOfClass
#undef kFirstWindowOfClass
static const WindowRef kFirstWindowOfClass = reinterpret_cast< WindowRef >( -1 );
#endif

#ifdef kLastWindowOfClass
#undef kLastWindowOfClass
static const WindowRef kLastWindowOfClass  = reinterpret_cast< WindowRef >( 0 );
#endif

#ifdef IsValidWindowRef
#undef IsValidWindowRef
inline pascal Boolean IsValidWindowRef( WindowRef w )  { return IsValidWindowPtr( w ); }
#endif

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( WindowManager );
	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	enum WindowDefProcID
	{
		documentProc    = ::documentProc,
		dBoxProc        = ::dBoxProc,
		plainDBox       = ::plainDBox,
		altDBoxProc     = ::altDBoxProc,
		noGrowDocProc   = ::noGrowDocProc,
		movableDBoxProc = ::movableDBoxProc,
		zoomDocProc     = ::zoomDocProc,
		zoomNoGrow      = ::zoomNoGrow,
		
		kWindowDefProcID_Max = nucleus::enumeration_traits< ::SInt16 >::max
	};
	
	enum WindowKind
	{
		kDialogWindowKind      = ::kDialogWindowKind,
		kApplicationWindowKind = ::kApplicationWindowKind,
		
		kWindowKind_Max = nucleus::enumeration_traits< ::SInt16 >::max
	};
	
	enum WindowPartCode
	{
		inDesk          = ::inDesk,
		inNoWindow      = ::inNoWindow,
		inMenuBar       = ::inMenuBar,
		inSysWindow     = ::inSysWindow,
		inContent       = ::inContent,
		inDrag          = ::inDrag,
		inGrow          = ::inGrow,
		inGoAway        = ::inGoAway,
		inZoomIn        = ::inZoomIn,
		inZoomOut       = ::inZoomOut,
		inCollapseBox   = ::inCollapseBox,
		inProxyIcon     = ::inProxyIcon,
		inToolbarButton = ::inToolbarButton,
		inStructure     = ::inStructure,
		
		kWindowPartCode_Max = nucleus::enumeration_traits< ::WindowPartCode >::max
	};
	
	enum PropertyCreator
	{
		kPropertyCreator_Max = nucleus::enumeration_traits< ::PropertyCreator >::max
	};
	
	enum PropertyTag
	{
		kPropertyTag_Max = nucleus::enumeration_traits< ::PropertyTag >::max
	};
	
	enum WindowClass
	{
		kAlertWindowClass        = ::kAlertWindowClass,
		kMovableAlertWindowClass = ::kMovableAlertWindowClass,
		kModalWindowClass        = ::kModalWindowClass,
		kMovableModalWindowClass = ::kMovableModalWindowClass,
		kFloatingWindowClass     = ::kFloatingWindowClass,
		kDocumentWindowClass     = ::kDocumentWindowClass,
		kUtilityWindowClass      = ::kUtilityWindowClass,
		kHelpWindowClass         = ::kHelpWindowClass,
		kSheetWindowClass        = ::kSheetWindowClass,
		kToolbarWindowClass      = ::kToolbarWindowClass,
		kPlainWindowClass        = ::kPlainWindowClass,
		kOverlayWindowClass      = ::kOverlayWindowClass,
		kSheetAlertWindowClass   = ::kSheetAlertWindowClass,
		kAltPlainWindowClass     = ::kAltPlainWindowClass,
		kDrawerWindowClass       = ::kDrawerWindowClass,
		kAllWindowClasses        = ::kAllWindowClasses,
		
		kWindowClass_Max = nucleus::enumeration_traits< ::WindowClass >::max
	};
	
	enum WindowAttributes
	{
		kWindowNoAttributes               = ::kWindowNoAttributes,
		kWindowCloseBoxAttribute          = ::kWindowCloseBoxAttribute,
		kWindowHorizontalZoomAttribute    = ::kWindowHorizontalZoomAttribute,
		kWindowVerticalZoomAttribute      = ::kWindowVerticalZoomAttribute,
		kWindowFullZoomAttribute          = ::kWindowFullZoomAttribute,
		kWindowCollapseBoxAttribute       = ::kWindowCollapseBoxAttribute,
		kWindowResizableAttribute         = ::kWindowResizableAttribute,
		kWindowSideTitlebarAttribute      = ::kWindowSideTitlebarAttribute,
		kWindowToolbarButtonAttribute     = ::kWindowToolbarButtonAttribute,
		kWindowNoUpdatesAttribute         = ::kWindowNoUpdatesAttribute,
		kWindowNoActivatesAttribute       = ::kWindowNoActivatesAttribute,
		kWindowOpaqueForEventsAttribute   = ::kWindowOpaqueForEventsAttribute,
		kWindowNoShadowAttribute          = ::kWindowNoShadowAttribute,
		kWindowHideOnSuspendAttribute     = ::kWindowHideOnSuspendAttribute,
		kWindowStandardHandlerAttribute   = ::kWindowStandardHandlerAttribute,
		kWindowHideOnFullScreenAttribute  = ::kWindowHideOnFullScreenAttribute,
		kWindowInWindowMenuAttribute      = ::kWindowInWindowMenuAttribute,
		kWindowLiveResizeAttribute        = ::kWindowLiveResizeAttribute,
		kWindowNoConstrainAttribute       = ::kWindowNoConstrainAttribute,
		kWindowStandardDocumentAttributes = ::kWindowStandardDocumentAttributes,
		kWindowStandardFloatingAttributes = ::kWindowStandardFloatingAttributes,
		
		kWindowAttributes_Max = nucleus::enumeration_traits< ::WindowAttributes >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( WindowAttributes )
	
	enum WindowPosition
	{
		kWindowNoPosition                      = ::kWindowNoPosition,
		kWindowDefaultPosition                 = ::kWindowDefaultPosition,
		kWindowCenterMainScreen                = ::kWindowCenterMainScreen,
		kWindowAlertPositionMainScreen         = ::kWindowAlertPositionMainScreen,
		kWindowStaggerMainScreen               = ::kWindowStaggerMainScreen,
		kWindowCenterParentWindow              = ::kWindowCenterParentWindow,
		kWindowAlertPositionParentWindow       = ::kWindowAlertPositionParentWindow,
		kWindowStaggerParentWindow             = ::kWindowStaggerParentWindow,
		kWindowCenterParentWindowScreen        = ::kWindowCenterParentWindowScreen,
		kWindowAlertPositionParentWindowScreen = ::kWindowAlertPositionParentWindowScreen,
		kWindowStaggerParentWindowScreen       = ::kWindowStaggerParentWindowScreen,
		
		kWindowPosition_Max = nucleus::enumeration_traits< ::UInt16 >::max
	};
	
	enum WindowPositionMethod
	{
		kWindowCenterOnMainScreen                = ::kWindowCenterOnMainScreen,
		kWindowCenterOnParentWindow              = ::kWindowCenterOnParentWindow,
		kWindowCenterOnParentWindowScreen        = ::kWindowCenterOnParentWindowScreen,
		kWindowCascadeOnMainScreen               = ::kWindowCascadeOnMainScreen,
		kWindowCascadeOnParentWindow             = ::kWindowCascadeOnParentWindow,
		kWindowCascadeOnParentWindowScreen       = ::kWindowCascadeOnParentWindowScreen,
		kWindowCascadeStartAtParentWindowScreen  = ::kWindowCascadeStartAtParentWindowScreen,
		kWindowAlertPositionOnMainScreen         = ::kWindowAlertPositionOnMainScreen,
		kWindowAlertPositionOnParentWindow       = ::kWindowAlertPositionOnParentWindow,
		kWindowAlertPositionOnParentWindowScreen = ::kWindowAlertPositionOnParentWindowScreen,
		
		kWindowPositionMethod_Max = nucleus::enumeration_traits< ::WindowPositionMethod >::max
	};
	
	enum WindowRegionCode
	{
		kWindowTitleBarRgn       = ::kWindowTitleBarRgn,
		kWindowTitleTextRgn      = ::kWindowTitleTextRgn,
		kWindowCloseBoxRgn       = ::kWindowCloseBoxRgn,
		kWindowZoomBoxRgn        = ::kWindowZoomBoxRgn,
		kWindowDragRgn           = ::kWindowDragRgn,
		kWindowGrowRgn           = ::kWindowGrowRgn,
		kWindowCollapseBoxRgn    = ::kWindowCollapseBoxRgn,
		kWindowTitleProxyIconRgn = ::kWindowTitleProxyIconRgn,
		kWindowStructureRgn      = ::kWindowStructureRgn,
		kWindowContentRgn        = ::kWindowContentRgn,
		kWindowUpdateRgn         = ::kWindowUpdateRgn,
		kWindowOpaqueRgn         = ::kWindowOpaqueRgn,
		kWindowGlobalPortRgn     = ::kWindowGlobalPortRgn,
		
		kWindowRegionCode_Max = nucleus::enumeration_traits< ::WindowRegionCode >::max
	};
	
   using ::GetWindowRegionRec;
   using ::GetWindowRegionPtr;
   using ::GetWindowRegionRecPtr;
   //using ::WindowRef;
   
   /* ... */
   
	#pragma mark -
	#pragma mark ¥ Specializations ¥
	
  }

#if OPAQUE_TOOLBOX_STRUCTS

namespace Nucleus
  {
   template <> struct Disposer< Nitrogen::WindowRef >: public std::unary_function< Nitrogen::WindowRef, void >
     {
      void operator()( Nitrogen::WindowRef w ) const
        {
         ::DisposeWindow( w );
        }
     };
  }

#endif

namespace Nitrogen
  {

   /* ... */

	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	// GetNewCWindow
	
	// 1402
	Nucleus::Owned< WindowRef > NewWindow( const Rect&       bounds,
	                                       ConstStr255Param  title,
	                                       bool              visible,
	                                       WindowDefProcID   procID,
	                                       WindowRef         behind,
	                                       bool              goAwayFlag,
	                                       RefCon            refCon );
	
	// GetNewWindow
	
	// 1437
	Nucleus::Owned< WindowRef > NewCWindow( const Rect&       bounds,
	                                        ConstStr255Param  title,
	                                        bool              visible,
	                                        WindowDefProcID   procID,
	                                        WindowRef         behind,
	                                        bool              goAwayFlag,
	                                        RefCon            refCon );
	
	// 1457
	inline void DisposeWindow( Nucleus::Owned< WindowRef > )  {}
	
	// CreateNewWindow
	// CreateWindowFromResource
	// StoreWindowIntoCollection
	// CreateWindowFromCollection
	// GetWindowOwnerCount
	// CloneWindow
	
	using ::GetWindowRetainCount;
	
	// RetainWindow
	// ReleaseWindow
	// CreateCustomWindow
	// ReshapeCustomWindow
	// RegisterWindowDefinition
	// GetWindowWidgetHilite
	
	inline bool IsValidWindowClass( WindowClass windowClass )  { return ::IsValidWindowClass( windowClass ); }
	
	inline WindowAttributes GetAvailableWindowAttributes( WindowClass windowClass )  { return WindowAttributes( ::GetAvailableWindowAttributes( windowClass ) ); }
	
	// GetWindowClass
	
	WindowAttributes GetWindowAttributes( WindowRef window );
	
	void ChangeWindowAttributes ( WindowRef         window,
	                              WindowAttributes  setTheseAttributes,
	                              WindowAttributes  clearTheseAttributes );
	
	// SetWindowClass (deprecated)
	// SetWindowModality
	// GetWindowModality
	// ShowFloatingWindows
	// HideFloatingWindows
	
	inline bool AreFloatingWindowsVisible()  { return ::AreFloatingWindowsVisible(); }
	
	// CreateWindowGroup
	// RetainWindowGroup
	// ReleaseWindowGroup
	
	using ::GetWindowGroupRetainCount;
	
	inline WindowGroupRef GetWindowGroupOfClass( WindowClass windowClass )  { return ::GetWindowGroupOfClass( windowClass ); }
	
	// SetWindowGroupName
	// CopyWindowGroupName
	// GetWindowGroupAttributes
	// ChangeWindowGroupAttributes
	// SetWindowGroupLevel
	// GetWindowGroupLevel
	// SendWindowGroupBehind
	
	using ::GetWindowGroup;
	
	// SetWindowGroup
	
	inline bool IsWindowContainedInGroup( WindowRef window, WindowGroupRef group )  { return ::IsWindowContainedInGroup( window, group ); }
	
	using ::GetWindowGroupParent;
	
	// SetWindowGroupParent
	
	inline WindowGroupRef GetWindowGroupSibling( WindowGroupRef group, bool isNext )  { return ::GetWindowGroupSibling( group, isNext ); }
	
	using ::GetWindowGroupOwner;
	
	// SetWindowGroupOwner
	// CountWindowGroupContents
	// GetWindowGroupContents
	// GetIndexedWindow
	// GetWindowIndex
	
	using ::ActiveNonFloatingWindow;
	
	inline bool IsWindowActive( WindowRef window )  { return ::IsWindowActive( window ); }
	
	// ActivateWindow
	// GetWindowActivationScope
	// SetWindowActivationScope
	
	using ::DebugPrintWindowGroup;
	using ::DebugPrintAllWindowGroups;
	
	// SetWindowContentColor
	// GetWindowContentColor
	// GetWindowContentPattern
	// SetWindowContentPattern
	// InstallWindowContentPaintProc
	// ScrollWindowRect
	// ScrollWindowRegion
	
	using ::ClipAbove;
	using ::PaintOne;
	using ::PaintBehind;
	using ::CalcVis;
	using ::CalcVisBehind;
	
	// CheckUpdate
	
	struct FindWindow_Result
	{
		operator WindowRef     () const  { return window; }
		operator WindowPartCode() const  { return part;   }
		
		WindowRef      window;
		WindowPartCode part;
	};
	typedef FindWindow_Result MacFindWindow_Result;
	
	// 3426
	FindWindow_Result MacFindWindow( Point point );
	
	using ::FrontWindow;
	using ::BringToFront;
	using ::SendBehind;
	using ::SelectWindow;
	using ::FrontNonFloatingWindow;
	
	// GetNextWindowOfClass
	// GetFrontWindowOfClass
	// FindWindowOfClass
	// CreateStandardWindowMenu
	// SetWindowAlternateTitle
	// CopyWindowAlternateTitle
	
	inline bool IsValidWindowPtr( WindowRef window )  { return ::IsValidWindowPtr( window ); }
	inline bool IsValidWindowRef( WindowRef window )  { return ::IsValidWindowRef( window ); }
	
	inline void HiliteWindow( WindowRef window, bool hilite )  { ::HiliteWindow( window, hilite ); }
	
	inline void SetWRefCon( WindowRef window, RefCon refCon )  { ::SetWRefCon( window, refCon ); }
	
	inline RefCon GetWRefCon( WindowRef window )  { return ::GetWRefCon( window ); }
	
	// SetWindowPic
	// GetWindowPic
	// GetWVariant
	// GetWindowFeatures
	// GetWindowRegion
	// GetWindowStructureWidths
	// BeginUpdate
	// EndUpdate
	// InvalWindowRgn
	
	// 3987
	void InvalWindowRect( WindowRef window, const Rect& bounds );
	
	// ValidWindowRgn
	// ValidWindowRect
	
	using ::DrawGrowIcon;
	
	// 4051
	void SetWTitle( WindowRef window, ConstStr255Param title );
	
	// 4065
	Str255 GetWTitle( WindowRef window );
	
	inline void SetWindowTitleWithCFString( WindowRef inWindow, CFStringRef inString )
	{
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		
		ThrowOSStatus( ::SetWindowTitleWithCFString( inWindow, inString ) );
	}
	
	inline Nucleus::Owned< CFStringRef > CopyWindowTitleAsCFString( WindowRef inWindow )
	{
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		
		CFStringRef result;
		ThrowOSStatus( ::CopyWindowTitleAsCFString( inWindow, &result ) );
		
		return Nucleus::Owned< CFStringRef >::Seize( result );
	}
	
	// SetWindowProxyFSSpec
	// GetWindowProxyFSSpec
	// SetWindowProxyAlias
	// GetWindowProxyAlias
	// SetWindowProxyCreatorAndType
	// GetWindowProxyIcon
	// SetWindowProxyIcon
	// RemoveWindowProxy
	// BeginWindowProxyDrag
	// EndWindowProxyDrag
	// TrackWindowProxyFromExistingDrag
	// TrackWindowProxyDrag
	// IsWindowModified
	// SetWindowModified
	// IsWindowPathSelectClick
	// WindowPathSelect
	// IsWindowPathSelectEvent
	// HiliteWindowFrameForDrag
	// TransitionWindow
	// TransitionWindowAndParent
	
	inline void MacMoveWindow( WindowRef  window,
	                           short      h,
	                           short      v,
	                           bool       front = false )
	{
		::MacMoveWindow( window, h, v, front );
	}
	
	inline void MacMoveWindow( WindowRef  window,
	                           Point      loc,
	                           bool       front = false )
	{
		::MacMoveWindow( window, loc.h, loc.v, front );
	}
	
	inline void SizeWindow( WindowRef  window,
	                        short      width,
	                        short      height,
	                        bool       updateFlag )
	{
		::SizeWindow( window, width, height, updateFlag );
	}
	
	inline void SizeWindow( WindowRef  window,
	                        Point      size,
	                        bool       updateFlag )
	{
		::SizeWindow( window, size.h, size.v, updateFlag );
	}
	
	// Previously GrowWindow_Result was a union with a Point member,
	// but that only works on big-endian.
	// The result of GrowWindow() is defined as:
	// * new height in high-order half
	// * new width in low-order half
	// * zero if no change
	
	struct GrowWindow_Result
	{
		long grew;
		
		operator long () const  { return grew; }
		
		operator Point() const  { return SetPt( grew & 0xffff, grew >> 16 ); }
	};
	
	// 4598
	GrowWindow_Result GrowWindow( WindowRef window, Point startPt, const Rect& bBox );
	
	GrowWindow_Result GrowWindow( WindowRef window, Point startPt );
	
	// 4614
	void DragWindow( WindowRef window, Point point, const Rect& dragRect );
	
	// ZoomWindow
	
	inline bool IsWindowCollapsable( WindowRef window )  { return ::IsWindowCollapsable( window ); }
	
	inline bool IsWindowCollapsed( WindowRef window )  { return ::IsWindowCollapsed( window ); }
	
	// CollapseWindow
	// CollapseAllWindows
	// CreateQDContextForCollapsedWindowDockTile
	// ReleaseQDContextForCollapsedWindowDockTile
	// UpdateCollapsedWindowDockTile
	
	inline Rect GetWindowBounds( WindowRef inWindow, WindowRegionCode regionCode )
	{
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		
		Rect result;
		ThrowOSStatus( ::GetWindowBounds( inWindow, regionCode, &result ) );
		
		return result;
	}
	
	// ResizeWindow
	
	inline void SetWindowBounds( WindowRef         inWindow,
	                             WindowRegionCode  regionCode,
	                             const Rect &      globalBounds )
	{
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		
		ThrowOSStatus( ::SetWindowBounds( inWindow, regionCode, &globalBounds ) );
	}
	
	// RepositionWindow
	// MoveWindowStructure
	// IsWindowInStandardState
	// ZoomWindowIdeal
	// GetWindowIdealUserState
	// SetWindowIdealUserState
	// GetWindowGreatestAreaDevice
	// ConstrainWindowToScreen
	// GetAvailableWindowPositioningBounds
	// GetAvailableWindowPositioningRegion
	
	using ::HideWindow;
	using ::MacShowWindow;
	
	inline void ShowHide( WindowRef window, bool showFlag )  { ::ShowHide( window, showFlag ); }
	
	inline bool MacIsWindowVisible( WindowRef window )  { return ::MacIsWindowVisible( window ); }
	
	// IsWindowLatentVisible
	// ShowSheetWindow
	// HideSheetWindow
	// GetSheetWindowParent
	// DisableScreenUpdates
	// EnableScreenUpdates
	// SetWindowAlpha
	// GetWindowAlpha
	// GetWindowProperty
	// GetWindowPropertySize
	// SetWindowProperty
	// RemoveWindowProperty
	// GetWindowPropertyAttributes
	// ChangeWindowPropertyAttributes
	// PinRect
	// GetGrayRgn
	// TrackBox
	
	using ::TrackGoAway;
	
	// DragGrayRgn
	// DragTheRgn
	
	using ::GetWindowList;
	using ::GetWindowPort;
	using ::GetWindowStructurePort;
	
	inline WindowKind GetWindowKind( WindowRef window )  { return WindowKind( ::GetWindowKind( window ) ); }
	
	// IsWindowHilited
	// IsWindowUpdatePending
	
	using ::MacGetNextWindow;
	using ::GetPreviousWindow;
	
	// GetWindowStandardState
	// GetWindowUserState
	
	inline void SetWindowKind( WindowRef window, WindowKind windowKind )  { ::SetWindowKind( window, windowKind ); }
	
	// SetWindowStandardState
	// SetWindowUserState
	
	using ::SetPortWindowPort;
	
	// GetWindowPortBounds
	
	using ::GetWindowFromPort;
	
	inline void SetWindowAttributes( WindowRef window, WindowAttributes setTheseAttributes )
	{
		Nitrogen::ChangeWindowAttributes( window, setTheseAttributes, kWindowNoAttributes );
	}

	inline void ClearWindowAttributes( WindowRef window, WindowAttributes clearTheseAttributes )
	{
		Nitrogen::ChangeWindowAttributes( window, kWindowNoAttributes, clearTheseAttributes );
	}
   
   /* ... */
   
   using ::ShowWindow;

   /* ... */
   
#if TARGET_RT_MAC_MACHO
/*ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ*/
/* ¥ Window Toolbars -- Tool bars are Mach-O only                                       */
/*ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ*/

	inline void SetWindowToolbar ( WindowRef inWindow, HIToolbarRef inToolbar ) {
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		ThrowOSStatus ( ::SetWindowToolbar ( inWindow, inToolbar ));
		}

	inline HIToolbarRef GetWindowToolbar ( WindowRef inWindow ) {
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		HIToolbarRef result;
		ThrowOSStatus ( ::GetWindowToolbar ( inWindow, &result ));
		return result;
		}

	inline void ShowHideWindowToolbar ( WindowRef inWindow, Boolean inShow, Boolean inAnimate ) {
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		ThrowOSStatus ( ::ShowHideWindowToolbar ( inWindow, inShow, inAnimate ));
		}

//	extern Boolean IsWindowToolbarVisible(WindowRef inWindow);
	using ::IsWindowToolbarVisible;
	
	inline void HIWindowSetProxyFSRef ( WindowRef inWindow, const FSRef &inRef ) {
		ThrowOSStatus ( ::HIWindowSetProxyFSRef ( inWindow, &inRef ));
		}
		
	inline FSRef HIWindowGetProxyFSRef ( WindowRef inWindow ) {
		::FSRef retVal;
		ThrowOSStatus ( ::HIWindowGetProxyFSRef ( inWindow, &retVal ));
		return retVal;
		}
#endif
		
	inline void SetWindowProxyAlias ( WindowRef inWindow, AliasHandle inAlias ) {
		ThrowOSStatus ( ::SetWindowProxyAlias ( inWindow, inAlias ));
		}
	
	inline Nucleus::Owned<AliasHandle> GetWindowProxyAlias ( WindowRef inWindow ) {
		AliasHandle result;
		ThrowOSStatus ( ::GetWindowProxyAlias ( inWindow, &result ));
		return Nucleus::Owned<AliasHandle>::Seize ( result );
		}

	inline void SetWindowProxyIcon ( WindowRef inWindow, IconRef icon ) {
		ThrowOSStatus ( ::SetWindowProxyIcon ( inWindow, icon ));
		}
	
	inline Nucleus::Owned<IconRef> GetWindowProxyIcon ( WindowRef inWindow ) {
		IconRef result;
		ThrowOSStatus ( ::GetWindowProxyIcon ( inWindow, &result ));
		return Nucleus::Owned<IconRef>::Seize ( result );
		}
	
	
	class WindowList_ContainerSpecifics
	{
		public:
			typedef WindowRef value_type;
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			
			value_type GetNextValue( value_type value )
			{
				return GetNextWindow( value );
			}
			
			static value_type begin_value()  { return GetWindowList(); }
	};
	
	class WindowList_Container: public Nucleus::LinkedListContainer< ::Nitrogen::WindowList_ContainerSpecifics >
	{
		friend WindowList_Container WindowList();
		
		private:
			WindowList_Container()
			: Nucleus::LinkedListContainer< ::Nitrogen::WindowList_ContainerSpecifics >( ::Nitrogen::WindowList_ContainerSpecifics() )
			{
			}
	};
	
	inline WindowList_Container WindowList()
	{
		return WindowList_Container();
	}
	
}

#endif
