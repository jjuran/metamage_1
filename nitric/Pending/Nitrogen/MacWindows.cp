// MacWindows.cp

#ifndef NITROGEN_MACWINDOWS_H
#include "Nitrogen/MacWindows.h"
#endif

#ifndef NITROGEN_SCOPED_H
#include "Nitrogen/Scoped.h"
#endif


namespace Nitrogen
  {
	
	Owned< WindowRef > NewWindow( const Rect&       bounds,
	                              ConstStr255Param  title,
	                              bool              visible,
	                              WindowDefProcID   procID,
	                              WindowRef         behind,
	                              bool              goAwayFlag,
	                              RefCon            refCon )
	{
		return Owned< WindowRef >::Seize
		(
			::NewWindow( NULL, &bounds, title, visible, procID, behind, goAwayFlag, refCon )
		);
	}
	
	Owned< WindowRef > NewCWindow( const Rect&       bounds,
	                              ConstStr255Param  title,
	                              bool              visible,
	                              WindowDefProcID   procID,
	                              WindowRef         behind,
	                              bool              goAwayFlag,
	                              RefCon            refCon )
	{
		return Owned< WindowRef >::Seize
		(
			::NewCWindow( NULL, &bounds, title, visible, procID, behind, goAwayFlag, refCon )
		);
	}
	
	FindWindow_Result MacFindWindow( Point point )
	{
		FindWindow_Result result;
		::WindowRef window;
		
		result.part = ::MacFindWindow( point, &window );
		result.window = window;
		
		return result;
	}
	
	void SetWTitle( WindowRef window, ConstStr255Param title )
	{
		return ::SetWTitle( window, title );
	}
	
	Str255 GetWTitle( WindowRef window )
	{
		::Str255 title;
		::GetWTitle( window, title );
		return title;
	}
	
	void SizeWindow( WindowRef window, short width, short height, bool updateFlag )
	{
		::SizeWindow( window, width, height, updateFlag );
	}
	
	static GrowWindow_Result GrowWindow_( WindowRef window, Point startPt, const Rect* bBox )
	{
		GrowWindow_Result result;
		result.grew = ::GrowWindow( window, startPt, bBox );
		return result;
	}
	
	GrowWindow_Result GrowWindow( WindowRef window, Point startPt, const Rect& bBox )
	{
		return GrowWindow_( window, startPt, &bBox );
	}
	
#if TARGET_API_MAC_CARBON
	
	GrowWindow_Result GrowWindow( WindowRef window, Point startPt )
	{
		return GrowWindow_( window, startPt, NULL );
	}
	
#endif
	
	void DragWindow( WindowRef window, Point point, const Rect& dragRect )
	{
		::DragWindow( window, point, &dragRect );
	}
	
	void SetWRefCon( WindowRef window, RefCon refCon )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		::SetWRefCon( window, refCon );
		
	#else
		
		reinterpret_cast< WindowPeek >( window.Get() )->refCon = refCon;
		
	#endif
	}
	
	RefCon GetWRefCon( WindowRef window )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		return ::GetWRefCon( window );
		
	#else
		
		return reinterpret_cast< WindowPeek >( window.Get() )->refCon;
		
	#endif
	}
	
	void InvalWindowRect( WindowRef window, const Rect& bounds )
	{
		if ( TARGET_API_MAC_CARBON || ::InvalWindowRect != NULL )
		{
			::InvalWindowRect( window, &bounds );
		}
		
	#if CALL_NOT_IN_CARBON
		
		else
		{
			Scoped< Port > savedPort;
			SetPortWindowPort( window );
			::InvalRect( &bounds );
		}
		
	#endif
	}
	
	WindowKind GetWindowKind( WindowRef window )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
			
		return ::GetWindowKind( window );
			
	#else
			
		return reinterpret_cast< WindowPeek >( window.Get() )->windowKind;
			
	#endif
	}
	
	void SetWindowKind( WindowRef window, WindowKind windowKind )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		::SetWindowKind( window, windowKind );
		
	#else
		
		reinterpret_cast< WindowPeek >( window.Get() )->windowKind = windowKind;
		
	#endif
	}
	
   WindowAttributes GetWindowAttributes( WindowRef window )
     {
      OnlyOnce< RegisterWindowManagerErrors >();
      ::WindowAttributes result;
      ThrowOSStatus( ::GetWindowAttributes( window, &result ) );
      return WindowAttributes( result );
     }

   void ChangeWindowAttributes ( WindowRef        window,
                                 WindowAttributes setTheseAttributes,
                                 WindowAttributes clearTheseAttributes )
     {
      OnlyOnce< RegisterWindowManagerErrors >();
      ThrowOSStatus( ::ChangeWindowAttributes( window, setTheseAttributes, clearTheseAttributes ) );
     }
   
   void RegisterWindowManagerErrors()
     {
      RegisterOSStatus< memFullErr >();
     }
  }
