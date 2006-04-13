// MacWindows.cp

#ifndef NITROGEN_MACWINDOWS_H
#include "Nitrogen/MacWindows.h"
#endif

#ifndef NUCLEUS_SCOPED_H
#include "Nucleus/Scoped.h"
#endif


namespace Nitrogen
  {
	
	Nucleus::Owned< WindowRef > NewWindow( const Rect&       bounds,
	                                       ConstStr255Param  title,
	                                       bool              visible,
	                                       WindowDefProcID   procID,
	                                       WindowRef         behind,
	                                       bool              goAwayFlag,
	                                       RefCon            refCon )
	{
		return Nucleus::Owned< WindowRef >::Seize( ::NewWindow( NULL,
		                                                        &bounds,
		                                                        title,
		                                                        visible,
		                                                        procID,
		                                                        behind,
		                                                        goAwayFlag,
		                                                        refCon ) );
	}
	
	Nucleus::Owned< WindowRef > NewCWindow( const Rect&       bounds,
	                                        ConstStr255Param  title,
	                                        bool              visible,
	                                        WindowDefProcID   procID,
	                                        WindowRef         behind,
	                                        bool              goAwayFlag,
	                                        RefCon            refCon )
	{
		return Nucleus::Owned< WindowRef >::Seize( ::NewCWindow( NULL,
		                                                         &bounds,
		                                                         title,
		                                                         visible,
		                                                         procID,
		                                                         behind,
		                                                         goAwayFlag,
		                                                         refCon ) );
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
	
	void SizeWindow( WindowRef  window,
	                 short      width,
	                 short      height,
	                 bool       updateFlag )
	{
		::SizeWindow( window, width, height, updateFlag );
	}
	
	namespace Detail
	{
	
		static GrowWindow_Result GrowWindow( WindowRef    window,
		                                     Point        startPt,
		                                     const Rect*  bBox )
		{
			GrowWindow_Result result;
			result.grew = ::GrowWindow( window, startPt, bBox );
			
			return result;
		}
		
	}
	
	GrowWindow_Result GrowWindow( WindowRef    window,
	                              Point        startPt,
	                              const Rect&  bBox )
	{
		return Detail::GrowWindow( window, startPt, &bBox );
	}
	
	GrowWindow_Result GrowWindow( WindowRef window, Point startPt )
	{
	#if TARGET_API_MAC_CARBON
		
		// Carbon allows you to pass a NULL Rect*
		return Detail::GrowWindow( window, startPt, NULL );
		
	#else
		
		const Rect unbounded = { -32767, -32767, 32767, 32767 };
		return Detail::GrowWindow( window, startPt, &unbounded );
		
	#endif
	}
	
	void DragWindow( WindowRef    window,
	                 Point        point,
	                 const Rect&  dragRect )
	{
		::DragWindow( window, point, &dragRect );
	}
	
	void SetWRefCon( WindowRef window, RefCon refCon )
	{
		::SetWRefCon( window, refCon );
	}
	
	RefCon GetWRefCon( WindowRef window )
	{
		return ::GetWRefCon( window );
	}
	
	void InvalWindowRect( WindowRef window, const Rect& bounds )
	{
		if ( TARGET_API_MAC_CARBON  ||  TARGET_CPU_PPC && ::InvalWindowRect != NULL )
		{
			::InvalWindowRect( window, &bounds );
		}
		
	#if CALL_NOT_IN_CARBON
		
		else
		{
			Nucleus::Scoped< Port > savedPort;
			SetPortWindowPort( window );
			
			::InvalRect( &bounds );
		}
		
	#endif
	}
	
	WindowKind GetWindowKind( WindowRef window )
	{
		return ::GetWindowKind( window );
	}
	
	void SetWindowKind( WindowRef window, WindowKind windowKind )
	{
		::SetWindowKind( window, windowKind );
	}
	
   WindowAttributes GetWindowAttributes( WindowRef window )
     {
      Nucleus::OnlyOnce< RegisterWindowManagerErrors >();
      ::WindowAttributes result;
      ThrowOSStatus( ::GetWindowAttributes( window, &result ) );
      return WindowAttributes( result );
     }

   void ChangeWindowAttributes ( WindowRef        window,
                                 WindowAttributes setTheseAttributes,
                                 WindowAttributes clearTheseAttributes )
     {
      Nucleus::OnlyOnce< RegisterWindowManagerErrors >();
      ThrowOSStatus( ::ChangeWindowAttributes( window, setTheseAttributes, clearTheseAttributes ) );
     }
   
   void RegisterWindowManagerErrors()
     {
      RegisterOSStatus< memFullErr >();
     }
  }
