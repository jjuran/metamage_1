/*
	CarbonEventLoop.cc
	------------------
*/

#include "CarbonEventLoop.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-qd-utils
#include "mac_qd/scoped_port.hh"

// NyanochromeCat
#include "About.hh"
#include "Offscreen.hh"
#include "Timer.hh"
#include "Window.hh"


#pragma exceptions off


#define LENGTH( array )  (sizeof (array) / sizeof *(array))


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
		if ( window == main_window )
		{
			QuitApplicationEventLoop();
		}
	}
	
	return err;
}

DEFINE_CARBON_UPP( EventHandler, window_WindowClose )

static EventTypeSpec windowClose_event[] =
{
	{ kEventClassWindow, kEventWindowClose },
};

static
pascal OSStatus application_About( EventHandlerCallRef  handler,
                                   EventRef             event,
                                   void*                userData )
{
	OSStatus err;
	
	HICommand command;
	err = GetEventParameter( event,
	                         kEventParamDirectObject,
	                         typeHICommand,
	                         NULL,
	                         sizeof (HICommand),
	                         NULL,
	                         &command );
	
	if ( err == noErr )
	{
		if ( command.commandID == kHICommandAbout )
		{
			show_About_box();
		}
		else
		{
			err = eventNotHandledErr;
		}
	}
	
	return err;
}

DEFINE_CARBON_UPP( EventHandler, application_About )

static EventTypeSpec about_event[] =
{
	{ kEventClassCommand, kEventCommandProcess },
};

static
pascal void periodic_draw( EventLoopTimerRef timer, void* data )
{
	prepare_next_frame();
	
	mac::qd::scoped_port saved_port( main_window );
	
	draw_window( main_window );
}

DEFINE_UPP( EventLoopTimer, periodic_draw )

static EventLoopTimerRef drawing_timer;

static
void install_draw_timer()
{
	OSStatus err;
	
	err = InstallEventLoopTimer( GetMainEventLoop(),
	                             0,
	                             1.0 / fps,  // seconds per frame
	                             UPP_ARG( periodic_draw ),
	                             NULL,
	                             &drawing_timer );
}

static
void remove_draw_timer()
{
	OSStatus err;
	
	err = RemoveEventLoopTimer( drawing_timer );
	
	drawing_timer = NULL;
}

static
pascal OSStatus application_RawKey( EventHandlerCallRef  handler,
                                    EventRef             event,
                                    void*                userData )
{
	OSStatus err;
	
	char c = 0;
	
	err = GetEventParameter( event,
	                         kEventParamKeyMacCharCodes,
	                         typeChar,
	                         NULL,
	                         sizeof c,
	                         NULL,
	                         &c );
	
	switch ( c )
	{
		case ' ':
			if ( drawing_timer )
			{
				remove_draw_timer();
			}
			else
			{
				install_draw_timer();
			}
			
			break;
		
		case 0x1C:
			prepare_prev_frame();
			draw_window( main_window );
			break;
		
		case 0x1D:
			prepare_next_frame();
			draw_window( main_window );
			break;
		
		default:
			break;
	}
	
	return noErr;
}

DEFINE_CARBON_UPP( EventHandler, application_RawKey )

static EventTypeSpec rawKey_event[] =
{
	{ kEventClassKeyboard, kEventRawKeyDown },
};

static
pascal OSStatus window_WindowClick( EventHandlerCallRef  handler,
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
		if ( window == main_window )
		{
			if ( drawing_timer )
			{
				remove_draw_timer();
			}
			else
			{
				install_draw_timer();
			}
		}
	}
	
	return err;
}

DEFINE_CARBON_UPP( EventHandler, window_WindowClick )

static EventTypeSpec windowClick_event[] =
{
	{ kEventClassWindow, kEventWindowHandleContentClick },
};

static
void install_window_event_handlers()
{
	OSStatus err;
	
	err = InstallWindowEventHandler( main_window,
	                                 UPP_ARG( window_WindowClose ),
	                                 1,
	                                 windowClose_event,
	                                 NULL,
	                                 NULL );
	
	err = InstallWindowEventHandler( main_window,
	                                 UPP_ARG( window_WindowClick ),
	                                 1,
	                                 windowClick_event,
	                                 NULL,
	                                 NULL );
}

void Carbon_event_loop()
{
	OSStatus err;
	
	install_draw_timer();
	
	install_window_event_handlers();
	
	EventHandlerRef aboutEventHandler;
	EventHandlerRef rawKeyEventHandler;
	
	err = InstallApplicationEventHandler( UPP_ARG( application_About ),
	                                      LENGTH( about_event ),
	                                      about_event,
	                                      NULL,
	                                      &aboutEventHandler );
	
	err = InstallApplicationEventHandler( UPP_ARG( application_RawKey ),
	                                      LENGTH( rawKey_event ),
	                                      rawKey_event,
	                                      NULL,
	                                      &rawKeyEventHandler );
	
	MenuRef appl = GetMenuHandle( 1 );
	MenuRef file = GetMenuHandle( 2 );
	
	SetMenuItemCommandID( appl, 1, kHICommandAbout );
	SetMenuItemCommandID( file, 3, kHICommandClose );
	
	RunApplicationEventLoop();
}
