// Nitrogen/MacWindows.h
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott, Joshua Juran, and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_MACWINDOWS_H
#define NITROGEN_MACWINDOWS_H

#ifndef NUCLEUS_ENUMERATION_H
#include "Nucleus/Enumeration.h"
#endif

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __MACWINDOWS__
#include FRAMEWORK_HEADER(HIToolbox,MacWindows.h)
#endif
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_ALIASES_H
#include "Nitrogen/Aliases.h"
#endif
#ifndef NITROGEN_APPLEEVENTS_H
#include "Nitrogen/AppleEvents.h"
#endif
#ifndef NITROGEN_COLLECTIONS_H
#include "Nitrogen/Collections.h"
#endif
#ifndef NITROGEN_DRAG_H
#include "Nitrogen/Drag.h"
#endif
#ifndef NITROGEN_EVENTS_H
#include "Nitrogen/Events.h"
#endif
#ifndef NITROGEN_MENUS_H
#include "Nitrogen/Menus.h"
#endif
#ifndef NITROGEN_MIXEDMODE_H
#include "Nitrogen/MixedMode.h"
#endif
#ifndef NITROGEN_QDOFFSCREEN_H
#include "Nitrogen/QDOffscreen.h"
#endif
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/Quickdraw.h"
#endif
#ifndef NITROGEN_TEXTCOMMON_H
#include "Nitrogen/TextCommon.h"
#endif
#ifndef NITROGEN_ICONS_H
#include "Nitrogen/Icons.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif


#ifdef kFirstWindowOfClass
#undef kFirstWindowOfClass
static const WindowRef kFirstWindowOfClass = reinterpret_cast< WindowRef >( -1 );
#endif

#ifdef kLastWindowOfClass
#undef kLastWindowOfClass
static const WindowRef kLastWindowOfClass  = reinterpret_cast< WindowRef >( 0 );
#endif

namespace Nitrogen
  {
	class WindowManagerErrorsRegistrationDependency
	{
		public:
			WindowManagerErrorsRegistrationDependency();
	};
	
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
		
		kWindowDefProcID_Max = Nucleus::Enumeration_Traits< ::SInt16 >::max
	};
	
	enum WindowKind
	{
		kDialogWindowKind      = ::kDialogWindowKind,
		kApplicationWindowKind = ::kApplicationWindowKind,
		
		kWindowKind_Max = Nucleus::Enumeration_Traits< ::SInt16 >::max
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
		
		kWindowPartCode_Max = Nucleus::Enumeration_Traits< ::WindowPartCode >::max
	};
	
   typedef Nucleus::Selector< class PropertyCreator_Tag, ::PropertyCreator >::Type PropertyCreator;

   typedef Nucleus::Selector< class PropertyTag_Tag, ::PropertyTag >::Type PropertyTag;

   typedef Nucleus::Selector< class WindowClass_Tag, ::WindowClass >::Type WindowClass;

   typedef Nucleus::Flag< class WindowAttributes_Tag, ::WindowAttributes >::Type WindowAttributes;
   
   NUCLEUS_DEFINE_FLAG_OPS( WindowAttributes )
   
   static const WindowAttributes kWindowNoAttributes = WindowAttributes( ::kWindowNoAttributes );

    typedef Nucleus::Selector< class WindowPositionMethod_Tag, ::WindowPositionMethod >::Type WindowPositionMethod;

   typedef Nucleus::Selector< class WindowRegionCode_Tag, ::WindowRegionCode >::Type WindowRegionCode;
  
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
	
	// 1402
	Nucleus::Owned< WindowRef > NewWindow( const Rect&       bounds,
	                                       ConstStr255Param  title,
	                                       bool              visible,
	                                       WindowDefProcID   procID,
	                                       WindowRef         behind,
	                                       bool              goAwayFlag,
	                                       RefCon            refCon );
	
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
	
	// 3777
	void SetWRefCon( WindowRef window, RefCon refCon );
	
	// 3791
	RefCon GetWRefCon( WindowRef window );
	
	// 3987
	void InvalWindowRect( WindowRef window, const Rect& bounds );
	
	// 4051
	void SetWTitle( WindowRef window, ConstStr255Param title );
	
	// 4065
	Str255 GetWTitle( WindowRef window );
	
	// 4580
	void SizeWindow( WindowRef  window,
	                 short      width,
	                 short      height,
	                 bool       updateFlag );
	
	union GrowWindow_Result
	{
		long grew;         // zero if no change
		Point dimensions;  // otherwise, the new dimensions
		
		operator long () const  { return grew;       }
		operator Point() const  { return dimensions; }
	};
	
	// 4598
	GrowWindow_Result GrowWindow( WindowRef window, Point startPt, const Rect& bBox );
	
	GrowWindow_Result GrowWindow( WindowRef window, Point startPt );
	
	// 4614
	void DragWindow( WindowRef window, Point point, const Rect& dragRect );
	
	// 6333
	WindowKind GetWindowKind( WindowRef window );
	
	// 6484
	void SetWindowKind( WindowRef window, WindowKind windowKind );
	
	// 3443
	using ::FrontWindow;
	
	// 3481
	using ::SelectWindow;
	
	// 4036
	using ::DrawGrowIcon;
	
	inline Nucleus::Owned<CFStringRef> CopyWindowTitleAsCFString ( WindowRef inWindow ) {
		(void) WindowManagerErrorsRegistrationDependency();
		CFStringRef result;
		ThrowOSStatus ( ::CopyWindowTitleAsCFString ( inWindow, &result ));
		return Nucleus::Owned<CFStringRef>::Seize ( result );
		}
	
	inline void SetWindowTitleWithCFString ( WindowRef inWindow, CFStringRef inString ) {
		(void) WindowManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::SetWindowTitleWithCFString ( inWindow, inString ));
		}
	
	// 5978
	using ::TrackGoAway;
	
	// 6301
	using ::GetWindowPort;
	
	inline void SetWindowBounds ( WindowRef inWindow, WindowRegionCode regionCode, const Rect &globalBounds ) {
		(void) WindowManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::SetWindowBounds ( inWindow, regionCode, &globalBounds ));
		}

	inline Rect GetWindowBounds ( WindowRef inWindow, WindowRegionCode regionCode ) {
		(void) WindowManagerErrorsRegistrationDependency();
		Rect retVal;
		ThrowOSStatus ( ::GetWindowBounds ( inWindow, regionCode, &retVal ));
		return retVal;
		}

	// 6551
	using ::SetPortWindowPort;
	
	// 6598
	using ::GetWindowFromPort;
	
   WindowAttributes GetWindowAttributes( WindowRef window );

   void ChangeWindowAttributes ( WindowRef        window,
                                 WindowAttributes setTheseAttributes,
                                 WindowAttributes clearTheseAttributes );
   
	inline void SetWindowAttributes ( WindowRef window, WindowAttributes setTheseAttributes ) {
		Nitrogen::ChangeWindowAttributes ( window, setTheseAttributes, kWindowNoAttributes );
		}

	inline void ClearWindowAttributes ( WindowRef window, WindowAttributes clearTheseAttributes ) {
		Nitrogen::ChangeWindowAttributes ( window, kWindowNoAttributes, clearTheseAttributes );
		}
   
   /* ... */
   
   using ::ShowWindow;

   /* ... */
   
#if TARGET_RT_MAC_MACHO
/*ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ*/
/* ¥ Window Toolbars -- Tool bars are Mach-O only                                       */
/*ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ*/

	inline void SetWindowToolbar ( WindowRef inWindow, HIToolbarRef inToolbar ) {
		(void) WindowManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::SetWindowToolbar ( inWindow, inToolbar ));
		}

	inline HIToolbarRef GetWindowToolbar ( WindowRef inWindow ) {
		(void) WindowManagerErrorsRegistrationDependency();
		HIToolbarRef result;
		ThrowOSStatus ( ::GetWindowToolbar ( inWindow, &result ));
		return result;
		}

	inline void ShowHideWindowToolbar ( WindowRef inWindow, Boolean inShow, Boolean inAnimate ) {
		(void) WindowManagerErrorsRegistrationDependency();
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

  }

#endif
