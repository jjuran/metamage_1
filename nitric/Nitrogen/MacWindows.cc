// Nitrogen/MacWindows.cc
// ----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/MacWindows.hh"

// MacGlue
#include "MacGlue/MacGlue.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace MacGlue
{
	
	DECLARE_MAC_GLUE( DisposeWindow );
	
}

namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( WindowManager )
	
	
	static void RegisterWindowManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class WindowManagerErrorsRegistration
	{
		public:
			WindowManagerErrorsRegistration()  { RegisterWindowManagerErrors(); }
	};
	
	static WindowManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	static       nucleus::disposer_class< WindowRef >::type gDisposeWindow;
	
#else
	
	static const nucleus::disposer_class< WindowRef >::type gDisposeWindow( &MacGlue::DisposeWindow );
	
#endif
	
	nucleus::owned< WindowRef > NewWindow( const Rect&       bounds,
	                                       ConstStr255Param  title,
	                                       bool              visible,
	                                       WindowDefProcID   procID,
	                                       WindowRef         behind,
	                                       bool              goAwayFlag,
	                                       RefCon            refCon )
	{
		return nucleus::owned< WindowRef >::seize( ::NewWindow( NULL,
		                                                        &bounds,
		                                                        title,
		                                                        visible,
		                                                        procID,
		                                                        behind,
		                                                        goAwayFlag,
		                                                        refCon ), gDisposeWindow );
	}
	
	nucleus::owned< WindowRef > NewCWindow( const Rect&       bounds,
	                                        ConstStr255Param  title,
	                                        bool              visible,
	                                        WindowDefProcID   procID,
	                                        WindowRef         behind,
	                                        bool              goAwayFlag,
	                                        RefCon            refCon )
	{
		return nucleus::owned< WindowRef >::seize( ::NewCWindow( NULL,
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
	
	void InvalWindowRect( WindowRef window, const Rect& bounds )
	{
		if ( TARGET_API_MAC_CARBON  ||  TARGET_CPU_PPC && ::InvalWindowRect != NULL )
		{
			::InvalWindowRect( window, &bounds );
		}
		
	#if CALL_NOT_IN_CARBON
		
		else
		{
			nucleus::saved< Port > savedPort;
			
			SetPortWindowPort( window );
			
			::InvalRect( &bounds );
		}
		
	#endif
	}
	
   WindowAttributes GetWindowAttributes( WindowRef window )
     {
      ::WindowAttributes result;
      Mac::ThrowOSStatus( ::GetWindowAttributes( window, &result ) );
      return WindowAttributes( result );
     }

   void ChangeWindowAttributes ( WindowRef        window,
                                 WindowAttributes setTheseAttributes,
                                 WindowAttributes clearTheseAttributes )
     {
      Mac::ThrowOSStatus( ::ChangeWindowAttributes( window, setTheseAttributes, clearTheseAttributes ) );
     }
   
   void RegisterWindowManagerErrors()
     {
      RegisterOSStatus< memFullErr >();
     }
  }
