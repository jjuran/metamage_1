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

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// MacGlue
#include "MacGlue/MacGlue.hh"

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef CARBON_CF_TYPES_CFTYPEREF_HH
#include "Carbon/CF/Types/CFTypeRef.hh"
#endif
#ifndef MAC_ALIASES_TYPES_ALIASHANDLE_HH
#include "Mac/Aliases/Types/AliasHandle.hh"
#endif
#ifndef MAC_ICONS_TYPES_ICONREF_HH
#include "Mac/Icons/Types/IconRef.hh"
#endif
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif
#ifndef MAC_WINDOWS_TYPES_WINDOWATTRIBUTES_HH
#include "Mac/Windows/Types/WindowAttributes.hh"
#endif
#ifndef MAC_WINDOWS_TYPES_WINDOWCLASS_HH
#include "Mac/Windows/Types/WindowClass.hh"
#endif
#ifndef MAC_WINDOWS_TYPES_WINDOWDEFPROCID_HH
#include "Mac/Windows/Types/WindowDefProcID.hh"
#endif
#ifndef MAC_WINDOWS_TYPES_WINDOWREF_HH
#include "Mac/Windows/Types/WindowRef.hh"
#endif

#ifndef NITROGEN_REFCON_HH
#include "Nitrogen/RefCon.hh"
#endif
#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
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
	#pragma mark ** Types **
	
	using Mac::WindowDefProcID;
	
	using Mac::documentProc;
	using Mac::dBoxProc;
	using Mac::plainDBox;
	using Mac::altDBoxProc;
	using Mac::noGrowDocProc;
	using Mac::movableDBoxProc;
	using Mac::zoomDocProc;
	using Mac::zoomNoGrow;
	
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
	
	using Mac::WindowClass;
	using Mac::WindowAttributes;
	using Mac::kWindowNoAttributes;
	
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
	
   /* ... */
   
	#pragma mark -
	#pragma mark ** Specializations **
	
  }

#if ! __LP64__

namespace MacGlue
{
	
	DECLARE_MAC_GLUE( DisposeWindow );
	
}

namespace Nitrogen
  {

   /* ... */
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	inline nucleus::disposer_class< WindowRef >::type Window_Disposer()
	{
		return nucleus::disposer_class< WindowRef >::type();
	}
	
#else
	
	inline nucleus::disposer_class< WindowRef >::type Window_Disposer()
	{
		return &MacGlue::DisposeWindow;
	}
	
#endif
	
	#pragma mark -
	#pragma mark ** Routines **
	
	// GetNewCWindow
	
	// 1402
	nucleus::owned< WindowRef > NewWindow( const Rect&       bounds,
	                                       ConstStr255Param  title,
	                                       bool              visible,
	                                       WindowDefProcID   procID,
	                                       WindowRef         behind,
	                                       bool              goAwayFlag,
	                                       long              refCon );
	
	inline nucleus::owned< WindowRef >
	//
	NewWindow( const Rect&       bounds,
	           ConstStr255Param  title,
	           bool              visible,
	           WindowDefProcID   procID,
	           WindowRef         behind,
	           bool              goAwayFlag,
	           const void*       refCon )
	{
		return NewWindow( bounds,
		                  title,
		                  visible,
		                  procID,
		                  behind,
		                  goAwayFlag,
		                  (long) refCon );  // reinterpret_cast
	}
	
	// GetNewWindow
	
	// 1437
	nucleus::owned< WindowRef > NewCWindow( const Rect&       bounds,
	                                        ConstStr255Param  title,
	                                        bool              visible,
	                                        WindowDefProcID   procID,
	                                        WindowRef         behind,
	                                        bool              goAwayFlag,
	                                        long              refCon );
	
	inline nucleus::owned< WindowRef >
	//
	NewCWindow( const Rect&       bounds,
	            ConstStr255Param  title,
	            bool              visible,
	            WindowDefProcID   procID,
	            WindowRef         behind,
	            bool              goAwayFlag,
	            const void*       refCon )
	{
		return NewCWindow( bounds,
		                   title,
		                   visible,
		                   procID,
		                   behind,
		                   goAwayFlag,
		                   (long) refCon );  // reinterpret_cast
	}
	
	// 1457
	inline void DisposeWindow( nucleus::owned< WindowRef > )  {}
	
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
	
	using ::SetWRefCon;
	
	inline void SetWRefCon( WindowRef window, const void* refCon )
	{
		::SetWRefCon( window, (long) refCon );  // reinterpret_cast
	}
	
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
	using ::SetWTitle;
	
	// 4065
	inline Str255 GetWTitle( WindowRef window )
	{
		::Str255 title;
		
		::GetWTitle( window, title );
		
		return title;
	}
	
	inline void SetWindowTitleWithCFString( WindowRef inWindow, CFStringRef inString )
	{
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		
		Mac::ThrowOSStatus( ::SetWindowTitleWithCFString( inWindow, inString ) );
	}
	
	inline nucleus::owned< CFStringRef > CopyWindowTitleAsCFString( WindowRef inWindow )
	{
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		
		CFStringRef result;
		Mac::ThrowOSStatus( ::CopyWindowTitleAsCFString( inWindow, &result ) );
		
		return nucleus::owned< CFStringRef >::seize( result );
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
		
		operator Point() const
		{
			Point point = { grew >> 16, grew & 0xffff };
			
			return point;
		}
	};
	
	// 4598
	GrowWindow_Result GrowWindow( WindowRef window, Point startPt, const Rect& bBox );
	
	GrowWindow_Result GrowWindow( WindowRef window, Point startPt );
	
	// 4614
	inline void DragWindow( WindowRef    window,
	                        Point        point,
	                        const Rect&  dragRect )
	{
		::DragWindow( window, point, &dragRect );
	}
	
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
		Mac::ThrowOSStatus( ::GetWindowBounds( inWindow, regionCode, &result ) );
		
		return result;
	}
	
	// ResizeWindow
	
	inline void SetWindowBounds( WindowRef         inWindow,
	                             WindowRegionCode  regionCode,
	                             const Rect &      globalBounds )
	{
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		
		Mac::ThrowOSStatus( ::SetWindowBounds( inWindow, regionCode, &globalBounds ) );
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

// Tool bars are Mach-O only

#pragma mark -
#pragma mark ** Window Toolbars **
#pragma mark -

	inline void SetWindowToolbar ( WindowRef inWindow, HIToolbarRef inToolbar ) {
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		Mac::ThrowOSStatus ( ::SetWindowToolbar ( inWindow, inToolbar ));
		}

	inline HIToolbarRef GetWindowToolbar ( WindowRef inWindow ) {
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		HIToolbarRef result;
		Mac::ThrowOSStatus ( ::GetWindowToolbar ( inWindow, &result ));
		return result;
		}

	inline void ShowHideWindowToolbar ( WindowRef inWindow, bool inShow, bool inAnimate ) {
		NUCLEUS_REQUIRE_ERRORS( WindowManager );
		Mac::ThrowOSStatus ( ::ShowHideWindowToolbar ( inWindow, inShow, inAnimate ));
		}

//	extern Boolean IsWindowToolbarVisible(WindowRef inWindow);
	using ::IsWindowToolbarVisible;
	
#ifdef MAC_OS_X_VERSION_10_4
	
	inline void HIWindowSetProxyFSRef ( WindowRef inWindow, const FSRef &inRef ) {
		Mac::ThrowOSStatus ( ::HIWindowSetProxyFSRef ( inWindow, &inRef ));
		}
		
	inline FSRef HIWindowGetProxyFSRef ( WindowRef inWindow ) {
		::FSRef retVal;
		Mac::ThrowOSStatus ( ::HIWindowGetProxyFSRef ( inWindow, &retVal ));
		return retVal;
		}
	
#endif
#endif
		
	inline void SetWindowProxyAlias ( WindowRef inWindow, AliasHandle inAlias ) {
		Mac::ThrowOSStatus ( ::SetWindowProxyAlias ( inWindow, inAlias ));
		}
	
	inline nucleus::owned<AliasHandle> GetWindowProxyAlias ( WindowRef inWindow ) {
		AliasHandle result;
		Mac::ThrowOSStatus ( ::GetWindowProxyAlias ( inWindow, &result ));
		return nucleus::owned<AliasHandle>::seize ( result );
		}

	inline void SetWindowProxyIcon ( WindowRef inWindow, IconRef icon ) {
		Mac::ThrowOSStatus ( ::SetWindowProxyIcon ( inWindow, icon ));
		}
	
	inline nucleus::owned<IconRef> GetWindowProxyIcon ( WindowRef inWindow ) {
		IconRef result;
		Mac::ThrowOSStatus ( ::GetWindowProxyIcon ( inWindow, &result ));
		return nucleus::owned<IconRef>::seize ( result );
		}
	
}

#endif  // #if ! __LP64__

#endif
