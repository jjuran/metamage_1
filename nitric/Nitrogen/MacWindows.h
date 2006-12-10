// MacWindows.h

#ifndef NITROGEN_MACWINDOWS_H
#define NITROGEN_MACWINDOWS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __MACWINDOWS__
#include FRAMEWORK_HEADER(HIToolbox,MacWindows.h)
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
	
	class WindowDefProcID_Tag {};
	typedef Nucleus::SelectorType< WindowDefProcID_Tag, ::SInt16, 0 > WindowDefProcID;
	
	class WindowKind_Tag {};
	typedef Nucleus::SelectorType< WindowKind_Tag, ::SInt16, 0 > WindowKind;
	
	class WindowPartCode_Tag {};
	typedef Nucleus::SelectorType< WindowPartCode_Tag, ::WindowPartCode, 0 > WindowPartCode;
	
   class PropertyCreator_Tag {};
   typedef Nucleus::SelectorType< PropertyCreator_Tag, ::PropertyCreator, '\?\?\?\?' > PropertyCreator;

   class PropertyTag_Tag {};      // Seems redundant, doesn't it?
   typedef Nucleus::SelectorType< PropertyTag_Tag, ::PropertyTag, '\?\?\?\?' > PropertyTag;

   class WindowClass_Tag {};
   typedef Nucleus::SelectorType< WindowClass_Tag, ::WindowClass, 0 > WindowClass;

   class WindowAttributes_Tag {};
   typedef Nucleus::FlagType< WindowAttributes_Tag, ::WindowAttributes, kWindowNoAttributes > WindowAttributes;

   class WindowPositionMethod_Tag {};
   typedef Nucleus::SelectorType< WindowPositionMethod_Tag, ::WindowPositionMethod, 0 > WindowPositionMethod;

   class WindowRegionCode_Tag {};
   typedef Nucleus::SelectorType< WindowRegionCode_Tag, ::WindowRegionCode, 0 > WindowRegionCode;
  
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
	
	// 5978
	using ::TrackGoAway;
	
	// 6301
	using ::GetWindowPort;
	
	// 6551
	using ::SetPortWindowPort;
	
	// 6598
	using ::GetWindowFromPort;
	
   WindowAttributes GetWindowAttributes( WindowRef window );

   void ChangeWindowAttributes ( WindowRef        window,
                                 WindowAttributes setTheseAttributes,
                                 WindowAttributes clearTheseAttributes );
   
	inline void SetWindowAttributes ( WindowRef window, WindowAttributes setTheseAttributes ) {
		Nitrogen::ChangeWindowAttributes ( window, setTheseAttributes, WindowAttributes::Make( 0 ) );
		}

	inline void ClearWindowAttributes ( WindowRef window, WindowAttributes clearTheseAttributes ) {
		Nitrogen::ChangeWindowAttributes ( window, WindowAttributes::Make( 0 ), clearTheseAttributes );
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
#endif
	
  }

#endif
