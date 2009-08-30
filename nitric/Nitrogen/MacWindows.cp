// Nitrogen/MacWindows.cp
// ----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/MacWindows.h"

// Nucleus
#include "Nucleus/Scoped.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( WindowManager )
	
	
	static void RegisterWindowManagerErrors();
	
	
#pragma force_active on
	
	class WindowManagerErrorsRegistration
	{
		public:
			WindowManagerErrorsRegistration()  { RegisterWindowManagerErrors(); }
	};
	
	static WindowManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
	
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	static       Nucleus::OwnedDefaults< WindowRef >::DisposerType gDisposeWindow;
	
#else
	
	namespace Function
	{
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		
		static pascal void DisposeWindow( WindowRef window )  { ::DisposeWindow( window ); }
		
	#else
		
		using ::DisposeWindow;
		
	#endif
	}
	
	static const Nucleus::OwnedDefaults< WindowRef >::DisposerType gDisposeWindow( &Function::DisposeWindow );
	
#endif
	
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
		                                                        refCon ), gDisposeWindow );
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
		                                                         refCon ), gDisposeWindow );
	}
	
	FindWindow_Result MacFindWindow( Point point )
	{
		FindWindow_Result result;
		::WindowRef window;
		
		result.part = WindowPartCode( ::MacFindWindow( point, &window ) );
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
		// Carbon allows you to pass a NULL Rect*
		const Rect* unbounded = NULL;
		
		if ( !TARGET_API_MAC_CARBON )
		{
			static const Rect reallyBigRect = { -32767, -32767, 32767, 32767 };
			
			unbounded = &reallyBigRect;
		}
		
		return Detail::GrowWindow( window, startPt, unbounded );
	}
	
	void DragWindow( WindowRef    window,
	                 Point        point,
	                 const Rect&  dragRect )
	{
		::DragWindow( window, point, &dragRect );
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
	
   WindowAttributes GetWindowAttributes( WindowRef window )
     {
      ::WindowAttributes result;
      ThrowOSStatus( ::GetWindowAttributes( window, &result ) );
      return WindowAttributes( result );
     }

   void ChangeWindowAttributes ( WindowRef        window,
                                 WindowAttributes setTheseAttributes,
                                 WindowAttributes clearTheseAttributes )
     {
      ThrowOSStatus( ::ChangeWindowAttributes( window, setTheseAttributes, clearTheseAttributes ) );
     }
   
   void RegisterWindowManagerErrors()
     {
      RegisterOSStatus< memFullErr >();
     }
  }
