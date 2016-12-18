/*
	WindowEventHandlers.cc
	----------------------
*/

#include "Pedestal/WindowEventHandlers.hh"

// Mac OS
#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif

// Pedestal
#include "Pedestal/WindowStorage.hh"


namespace Pedestal
{
	
#if ! TARGET_RT_MAC_CFM
	
	/*
		Null definitions for classic 68K and Mach-O.
		(UPPs are plain functions, not routine descriptors.)
	*/
	
	#define DEFINE_UPP2( UPPTypeName, proc )  /**/
	
	#define UPP_ARG( proc )  &proc
	
#else
	
	/*
		CFM:  Create a routine descriptor at static initialization time.
	*/
	
	#define UPP_VARNAME( proc )  proc ## _upp
	
	#define DEFINE_UPP2( UPPTypeName, proc )  \
	static UPPTypeName UPP_VARNAME( proc ) = New ## UPPTypeName( &proc );
	
	#define UPP_ARG( proc )  UPP_VARNAME( proc )
	
#endif
	
	#define DEFINE_UPP( BaseName, proc )  DEFINE_UPP2( BaseName ## UPP, proc )
	
#if ! TARGET_RT_MAC_CFM  ||  TARGET_API_MAC_CARBON
	
	/*
		For non-CFM, use the null macro above.
		For Carbon CFM, create a routine descriptor as usual.
	*/
	
	#define DEFINE_CARBON_UPP( BaseName, proc )  DEFINE_UPP( BaseName, proc )
	
#else
	
	/*
		For non-Carbon CFM, declare the UPP variable extern, but don't
		define it.  Since these are Carbon event handlers, installing them
		would be a run-time error, so make it a link error instead, and don't
		waste resources creating the routine descriptors.
	*/
	
	#define DEFINE_CARBON_UPP( BaseName, proc )  \
	extern BaseName ## UPP UPP_VARNAME( proc );
	
#endif
	
	
	static
	pascal OSStatus window_WindowClose( EventHandlerCallRef  handler,
	                                    EventRef             event,
	                                    void*                userData )
	{
		OSStatus err;
		
		WindowRef window;
		err = GetEventParameter( event,
		                         kEventParamDirectObject,
		                         typeWindowRef,
		                         NULL,
		                         sizeof (WindowRef),
		                         NULL,
		                         &window );
		
		if ( err == noErr )
		{
			close_window( window );
		}
		
		return err;
	}
	
	static EventTypeSpec windowClose_event[] =
	{
		{ kEventClassWindow, kEventWindowClose },
	};
	
	DEFINE_CARBON_UPP( EventHandler, window_WindowClose )
	
	
	OSStatus install_window_event_handlers( WindowRef window )
	{
		OSStatus err;
		
		err = InstallWindowEventHandler( window,
		                                 UPP_ARG( window_WindowClose ),
		                                 1,
		                                 windowClose_event,
		                                 NULL,
		                                 NULL );
		
		return err;
	}
	
}
