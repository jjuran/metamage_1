/*	==============
 *	Application.cc
 *	==============
 */

#include "Pedestal/Application.hh"

// Standard C++
#include <algorithm>

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __CONTROLS__
#include <Controls.h>
#endif
#ifdef __MACOS__
#ifndef __DISKINIT__
#include <DiskInit.h>
#endif
#endif
#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif

// Nostalgia
#include "Nostalgia/LowMem.hh"

// mac-config
#include "mac_config/adb.hh"
#include "mac_config/apple-events.hh"
#include "mac_config/upp-macros.hh"

// mac-sys-utils
#include "mac_sys/async_wakeup.hh"
#include "mac_sys/current_process.hh"
#include "mac_sys/gestalt.hh"
#include "mac_sys/is_front_process.hh"

// mac-qd-utils
#include "mac_qd/assign_pixel_rgn.hh"

// mac-app-utils
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// Debug
#include "debug/assert.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

#include "Nitrogen/AEInteraction.hh"
#include "Nitrogen/Controls.hh"
#include "Nitrogen/MacErrors.hh"
#include "Nitrogen/MacWindows.hh"
#include "Nitrogen/Menus.hh"
#include "Nitrogen/Quickdraw.hh"

// Pedestal
#include "Pedestal/ADBKeyboard.hh"
#include "Pedestal/ClickTarget.hh"
#include "Pedestal/Commands.hh"
#include "Pedestal/Initialize.hh"
#include "Pedestal/MenuBar.hh"
#include "Pedestal/TrackControl.hh"
#include "Pedestal/Quasimode.hh"
#include "Pedestal/View.hh"
#include "Pedestal/Window.hh"
#include "Pedestal/WindowEventHandlers.hh"
#include "Pedestal/WindowMenu.hh"
#include "Pedestal/WindowStorage.hh"


namespace Nitrogen
{
	
	using ::BeginUpdate;
	using ::EndUpdate;
	
	class Update_Scope
	{
		private:
			WindowRef window;
		
		public:
			Update_Scope( WindowRef window ) : window( window )
			{
				BeginUpdate( window );
			}
			
			~Update_Scope()
			{
				EndUpdate( window );
			}
	};
	
}

namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	using Mac::kCoreEventClass;
	using Mac::kAEQuitApplication;
	
	const uint32_t gestaltAppleEventsAttr = 'evnt';
	
	static const UInt32 kEitherShiftKey   = shiftKey   | rightShiftKey;
	static const UInt32 kEitherOptionKey  = optionKey  | rightOptionKey;
	static const UInt32 kEitherControlKey = controlKey | rightControlKey;
	
	static MenuRef the_Window_menu;
	
	
	const bool apple_events_present =
		CONFIG_APPLE_EVENTS  &&
			(CONFIG_APPLE_EVENTS_GRANTED  ||
				mac::sys::gestalt( gestaltAppleEventsAttr ) != 0);
	
	
	static bool gInForeground;     // set to true when the app is frontmost
	static bool gEndOfEventLoop;   // set to true once the app is ready to exit
	
	
	static UInt32 gTickCountAtLastContextSwitch = 0;
	
	// ADB address of the keyboard from the last key-down event.
	static SInt8 gLastKeyboard;  // ADBAddress
	
	static bool gNeedToConfigureKeyboard = false;
	
	static bool gShiftKeyIsDownFromKeyStroke = false;
	
	static UInt32 gShiftSpaceQuasimodeMask = 0;
	
	static boost::intrusive_ptr< Quasimode > gQuasimode;
	
	
	static inline void DebugBeep()
	{
		Mac::SysBeep();
	}
	
	
	void (*gThreadYield_Hook )() = NULL;
	bool (*gActivelyBusy_Hook)() = NULL;
	bool (*gReadyToExit_Hook )() = NULL;
	
	static bool tapping_key_events;
	
	void set_key_event_tap( bool enabled )
	{
		if ( enabled == tapping_key_events )
		{
			return;
		}
		
		tapping_key_events = enabled;
		
		if ( TARGET_API_MAC_CARBON )
		{
			mac::app::set_Aqua_menu_key( kHICommandQuit, 'Q' * ! enabled );
			mac::app::set_Aqua_menu_key( kHICommandHide, 'H' * ! enabled );
		}
	}
	
	
	static
	void ThreadYield()
	{
		if ( gThreadYield_Hook )
		{
			gThreadYield_Hook();
		}
	}
	
	static bool ActivelyBusy()
	{
		return gActivelyBusy_Hook ? gActivelyBusy_Hook() : false;
	}
	
	static bool ReadyToExit()
	{
		return gReadyToExit_Hook ? gReadyToExit_Hook() : true;
	}
	
	static
	pascal OSErr Quit( const ::AppleEvent* event, ::AppleEvent* reply, SInt32 )
	{
		Quit();
		
		return noErr;
	}
	
	static
	View* get_window_view_ready( WindowRef window )
	{
		if ( window != NULL )
		{
			if ( View* view = get_window_view( window ) )
			{
				SetPortWindowPort( window );
				
				return view;
			}
		}
		
		return NULL;
	}
	
	static bool DoCommand( CommandCode code );
	
	static bool DispatchMenuItem( CommandCode code )
	{
		bool handled = false;
		
		if ( View* view = get_window_view_ready( FrontWindow() ) )
		{
			handled = view->UserCommand( code );
		}
		
		handled = handled || DoCommand( CommandCode( code ) );
		
		if ( !handled )
		{
			DebugBeep();  // FIXME
		}
		
		return handled;
	}
	
	enum SuspendResume_flag
	{
		Flag_suspending,
		Flag_resuming,
	};
	
	static inline
	SuspendResume_flag get_SuspendResume_flag( const EventRecord& event )
	{
		const bool resuming = event.message & resumeFlag;
		
		return SuspendResume_flag( resuming );
	}
	
	static inline
	SInt8 GetKeyboardFromEvent( const EventRecord& event )
	{
		return (event.message & adbAddrMask) >> 16;
	}
	
	static
	void SuspendResume( SuspendResume_flag flag )
	{
		if ( CONFIG_ADB )
		{
			gNeedToConfigureKeyboard = true;
		}
		
		if ( WindowRef front = FrontWindow() )
		{
			window_activated( front, flag != Flag_suspending );
		}
	}
	
	/*
	 *	--------------------------
	 *	Event processing routines.
	 *	--------------------------
	 */
	
	static void HandleMenuChoice( long menuChoice );
	
	static
	void DispatchCursor( const EventRecord& event )
	{
		if ( View* view = get_window_view_ready( FrontWindow() ) )
		{
			if ( view->SetCursor( event ) )
			{
				return;
			}
		}
		
		N::SetCursor( N::GetQDGlobalsArrow() );
	}
	
	static void DispatchHighLevelEvent( const EventRecord& event )
	{
		ASSERT( event.what == kHighLevelEvent );
		
		OSErr err = AEProcessAppleEvent( &event );
		
		if ( err != errAEEventNotHandled )
		{
			Mac::ThrowOSStatus( err );
		}
	}
	
	static inline
	void RespondToDrag( const EventRecord& event, WindowRef window )
	{
	#if TARGET_API_MAC_CARBON
		
		const Rect& bounds = N::GetQDGlobalsScreenBits().bounds;
		
	#else
		
		const Rect& bounds = qd.screenBits.bounds;
		
	#endif
		
		N::DragWindow( window, event.where, bounds );
	}
	
	static bool TrackedControl( ControlRef control, N::ControlPartCode part, Point point )
	{
		// Did we actually hit a control?
		if ( part != N::kControlNoPart )
		{
			// If the control has an action routine, it's not a Pedestal control.
			// It might (for example) be a List-Manager-owned scrollbar, 
			// which we trigger through LClick().
			// Return false to indicate that we didn't handle the event.
			if ( N::GetControlAction( control ) == NULL )
			{
				Control_UserData* userData = N::GetControlReference( control );
				
				if ( userData != NULL )
				{
					ControlTracker trackControl = userData->trackHook;
					
					if ( trackControl != NULL )
					{
						trackControl( control, part, point );
					}
					
					return true;
				}
			}
		}
		
		return false;
	}
	
	static inline bool TrackedControl( const N::FindControl_Result& found, Point point )
	{
		return TrackedControl( found.control, found.part, point );
	}
	
	static void RespondToContent( const EventRecord& event, WindowRef window )
	{
		Point pt = N::GlobalToLocal( event.where );
		
		// TrackedControl's result indicates whether a control was found.
		if ( TrackedControl( N::FindControl( pt, window ), pt ) )
		{
			// already handled
		}
		else
		{
			window_mouseDown( window, event );
		}
	}
	
	static void RespondToGrow( const EventRecord& event, WindowRef window )
	{
		Rect sizeRect = { 30, 50, 10000, 10000 };
		
		Point grown = N::GrowWindow( window, event.where, sizeRect );
		
		if ( grown.h != 0  ||  grown.v != 0 )
		{
			if ( N::GetWindowKind( window ) == N::kApplicationWindowKind )
			{
				ResizeWindow( window, grown );
			}
		}
	}
	
	static void RespondToGoAway( const EventRecord& event, WindowRef window )
	{
		if ( N::TrackGoAway( window, event.where ) )
		{
			close_window( window );
		}
	}
	
	static
	void DispatchMouseUp( const EventRecord& event )
	{
		if ( View* target = Get_ClickTarget() )
		{
			target->MouseUp( event );
			
			Reset_ClickTarget();
		}
	}
	
	static void DispatchMouseDown( const EventRecord& event )
	{
		ASSERT( event.what == mouseDown );
		
		N::FindWindow_Result found = N::FindWindow( event.where );
		
		if ( found.part == N::inMenuBar )
		{
			populate_Window_menu( the_Window_menu );
			
			HandleMenuChoice( ::MenuSelect( event.where ) );
			
			return;
		}
		
		if ( found.window == NULL )
		{
			// Sometimes happens, though I'm not sure under what circumstances.
			return;
		}
		
	#if CALL_NOT_IN_CARBON
		
		if ( found.part == inSysWindow )
		{
			::SystemClick( &event, found.window );
			
			return;
		}
		
	#endif
		
		N::SetPortWindowPort( found.window );
		
		switch ( found.part )
		{
			case inDrag:     RespondToDrag   ( event, found.window );  break;
			case inContent:  RespondToContent( event, found.window );  break;
			case inGrow:     RespondToGrow   ( event, found.window );  break;
			case inGoAway:   RespondToGoAway ( event, found.window );  break;
			
			default:
				break;
		}
	}
	
	static inline bool CharIsArrowKey( char c )
	{
		return (c & 0xFC) == 0x1C;
	}
	
	static inline bool CharIsDelete( char c )
	{
		return c == kBackspaceCharCode  ||  c == kDeleteCharCode;
	}
	
	static inline bool CharMayBeCommand( char c )
	{
		// Command-Space is passed as a key-down, never a menu command.
		// Command-arrow is an editing gesture, not a command.
		// Command-Delete is also an editing gesture.
		
		return c != ' '  &&  c != '.'  &&  !CharIsArrowKey( c )  &&  !CharIsDelete( c );
	}
	
	static inline bool ShouldEnterShiftSpaceQuasiMode( const EventRecord& event )
	{
		if ( gQuasimode                   )  return false;
		if ( gShiftKeyIsDownFromKeyStroke )  return false;
		
		const char c = event.message & charCodeMask;
		
		if ( c != ' ' )  return false;
		
		if ( event.modifiers & cmdKey )  return false;
		
		bool leftShift  = event.modifiers & shiftKey;
		bool rightShift = event.modifiers & rightShiftKey;
		
		return leftShift != rightShift;
	}
	
	static void EnterShiftSpaceQuasimode( const EventRecord& event )
	{
		if ( View* view = get_window_view_ready( FrontWindow() ) )
		{
			if (( gQuasimode = view->EnterShiftSpaceQuasimode( event ) ))
			{
				gShiftSpaceQuasimodeMask = event.modifiers & kEitherShiftKey;
				
				return;
			}
		}
		
		Mac::SysBeep();
	}
	
	static void DispatchKey( const EventRecord& event )
	{
		ASSERT( event.what >= keyDown  &&  event.what <= autoKey );
		
		if ( CONFIG_ADB )
		{
			gLastKeyboard = GetKeyboardFromEvent( event );
		}
		
		if ( event.what == keyUp  ||  tapping_key_events )
		{
			// Genie's eventtap view calls KeyDown() from KeyUp() anyway.
			
			if ( View* view = get_window_view_ready( FrontWindow() ) )
			{
				view->KeyUp( event );
			}
			
			return;
		}
		
		const char c = event.message & charCodeMask;
		
		bool command = event.modifiers & cmdKey;
		
		if ( command )
		{
			// no commands on autoKey
			if ( event.what != keyDown )
			{
				return;
			}
			
			// Commands cancel Shift-Space quasimode
			gQuasimode.reset();
		}
		
		if ( command  &&  CharMayBeCommand( c ) )
		{
			HandleMenuChoice( ::MenuKey( c ) );
		}
		else if ( gQuasimode && gQuasimode->KeyDown( event ) )
		{
			// done
		}
		else if ( ShouldEnterShiftSpaceQuasiMode( event ) )
		{
			EnterShiftSpaceQuasimode( event );
		}
		else if ( View* view = get_window_view_ready( FrontWindow() ) )
		{
			view->KeyDown( event );
		}
		
		gShiftKeyIsDownFromKeyStroke = event.modifiers & kEitherShiftKey;
	}
	
	static void DispatchActivate( const EventRecord& event )
	{
		WindowRef window = (WindowRef) event.message;
		
		SetPortWindowPort( window );
		
		window_activated( window, event.modifiers & activeFlag );
	}
	
	static void DispatchUpdate( const EventRecord& event )
	{
		WindowRef window = (WindowRef) event.message;
		
		ASSERT( window != NULL );
		
		N::Update_Scope update( window );
		
		if ( ::IsPortVisibleRegionEmpty( N::GetWindowPort( window ) ) )
		{
			return;
		}
		
		{
			SetPortWindowPort( window );
			
			window_update( window );
			
			n::saved< N::Clip > savedClip;
			
			N::ClipRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
			
			N::UpdateControls( window );
		}
	}
	
	static void DispatchDiskInsert( const EventRecord& event )
	{
	#if CALL_NOT_IN_CARBON
		
		long message = event.message;
		OSErr err = message >> 16;
		
		if ( err != noErr )
		{
			::DILoad();
			
			const Point pt = { 120, 120 };
			err = ::DIBadMount( pt, message );
			
			::DIUnload();
		}
		
	#endif
	}
	
	static void DispatchOSEvent( const EventRecord& event )
	{
		switch ( (event.message & osEvtMessageMask) >> 24 )
		{
			case suspendResumeMessage:
				{
					SuspendResume_flag flag = get_SuspendResume_flag( event );
					
					gInForeground = flag != Flag_suspending;
					
					SuspendResume( flag );
				}
				break;
			
			case mouseMovedMessage:
				break;
			
			default:
				break;
		}
	}
	
	
	static void DispatchEvent( const EventRecord& event )
	{
		switch ( event.what )
		{
		//	case nullEvent:        DispatchNullEvent     ( event );  break;
			
		#if CONFIG_APPLE_EVENTS
		
			case kHighLevelEvent:  DispatchHighLevelEvent( event );  break;
			
		#endif
			
			case mouseDown:        DispatchMouseDown     ( event );  break;
			case mouseUp:          DispatchMouseUp       ( event );  break;
			case keyDown:
			case keyUp:
				case autoKey:      DispatchKey           ( event );  break;
			case activateEvt:      DispatchActivate      ( event );  break;
			case updateEvt:        DispatchUpdate        ( event );  break;
			case diskEvt:          DispatchDiskInsert    ( event );  break;
			case osEvt:            DispatchOSEvent       ( event );  break;
			
			default:
				break;
		}
	}
	
	static void GiveIdleTimeToWindows( const EventRecord& event )
	{
		n::saved< N::Port > savePort;
		
		// FIXME:  Use window iterator
		for ( WindowRef window = N::FrontWindow();
		      window != NULL;
		      //window = N::GetNextWindow( window ) )  // FIXME
		      window = ::GetNextWindow( window ) )
		{
			if ( View* view = get_window_view_ready( window ) )
			{
				view->Idle( event );
			}
		}
	}
	
	Application::Application()
	{
		Init_Memory( 0 );
		
		mac::app::init_toolbox();
		mac::app::install_menus();
		
		if ( apple_events_present )
		{
			DEFINE_UPP( AEEventHandler, Quit );
			
			OSErr err;
			
			err = AEInstallEventHandler( kCoreEventClass,
			                             kAEQuitApplication,
			                             UPP_ARG( Quit ),
			                             0,
			                             false );
		}
		
		MenuRef appleMenu  = GetMenuHandle( 1 );
		MenuRef fileMenu   = GetMenuHandle( 2 );
		MenuRef editMenu   = GetMenuHandle( 3 );
		MenuRef windowMenu = GetMenuHandle( 4 );
		
		the_Window_menu = windowMenu;
		
		AddMenu( appleMenu );
		AddMenu( fileMenu  );
		AddMenu( editMenu  );
		AddMenu( windowMenu );
		
		FixUpAboutMenuItem( appleMenu );
	}
	
	
	static void CheckKeyboard()
	{
		if ( gNeedToConfigureKeyboard  &&  gLastKeyboard != 0 )
		{
			// Don't reconfigure the keyboard if certain modifiers are down,
			// since that confuses the OS
			UInt32 confusingModifiers =   kEitherShiftKey
			                            | kEitherOptionKey
			                            | kEitherControlKey;
			
			const UInt32 currentKeyModifiers = ::GetCurrentKeyModifiers();
			
			if ( (currentKeyModifiers & confusingModifiers) == 0 )
			{
				const bool capsLock_on = currentKeyModifiers & alphaLock;
				
				bool active = gInForeground  &&  ! gEndOfEventLoop;
				
				ConfigureKeyboard( gLastKeyboard, active, capsLock_on );
				
				gNeedToConfigureKeyboard = false;
			}
		}
	}
	
	static bool gEventCheckNeeded = false;
	
	static bool ReadyToWaitForEvents()
	{
		if ( gEventCheckNeeded )
		{
			return true;
		}
		
		const UInt32 gMaxTicksBetweenEventChecks = 6;
		
		const UInt32 timetoWNE = gTickCountAtLastContextSwitch + gMaxTicksBetweenEventChecks;
		
		UInt32 now = ::LMGetTicks();
		
		bool readyToWait = now >= timetoWNE;
		
		return readyToWait;
	}
	
	static void CheckShiftSpaceQuasiMode( const EventRecord& event )
	{
		if ( !(event.modifiers & kEitherShiftKey) )
		{
			gShiftKeyIsDownFromKeyStroke = false;
		}
		
		if ( (event.modifiers & gShiftSpaceQuasimodeMask) != gShiftSpaceQuasimodeMask )
		{
			gQuasimode.reset();
			
			gShiftSpaceQuasimodeMask = 0;
		}
		
		if ( gShiftSpaceQuasimodeMask != 0 )
		{
			// Don't sleep long while we're in a quasimode, so we can detect
			// when the key goes up.
			
			// 4 ticks does the trick unless you're deliberately trying to see
			// it fail, instead of actually trying to work.  And even then it's
			// only occasionally, and if your finger leaves the key, never.
			
			AdjustSleepForTimer( 4 );
		}
	}
	
	static const UInt32 gMaxTicksBetweenNonZeroSleeps = 30;
	
	static UInt32 gTicksAtLastTrueSleep = 0;
	
	static UInt32 gTicksAtNextBusiness = 0;
	
	static bool gIdleNeeded = false;
	
	static EventRecord WaitNextEvent( UInt32 sleep, RgnHandle mouseRgn = NULL )
	{
		static Point last_global_mouse = { 0, 0 };
		
		static RgnHandle current_mouse_location = NewRgn();
		
		if ( mouseRgn == NULL )
		{
			mouseRgn = current_mouse_location;
		}
		
		EventRecord event;
		
		(void) ::WaitNextEvent( everyEvent, &event, sleep, mouseRgn );
		
		mac::sys::clear_async_wakeup();
		
		last_global_mouse = event.where;
		
		mac::qd::assign_pixel_rgn( current_mouse_location, last_global_mouse );
		
		return event;
	}
	
	static EventRecord GetAnEvent()
	{
		const UInt32 now = ::LMGetTicks();
		
		UInt32 ticksToSleep = 0x7FFFFFFF;
		
		// If we're actively busy (i.e. some thread is in Breathe()), sleep for
		// at most one tick, and that only if it's been a while since.
		
		if ( ActivelyBusy() )
		{
			const bool nonzero = now >= gTicksAtLastTrueSleep + gMaxTicksBetweenNonZeroSleeps;
			
			ticksToSleep = nonzero ? 1 : 0;  // (little to) no sleep for the busy
		}
		else if ( gIdleNeeded )
		{
			ticksToSleep = 1;
		}
		
		gTicksAtNextBusiness = std::max( gTicksAtNextBusiness, now );
		
		ticksToSleep = std::min( ticksToSleep, gTicksAtNextBusiness - now );
		
		gTicksAtNextBusiness = 0xffffffff;
		
		EventRecord nextEvent = WaitNextEvent( ticksToSleep );
		
		if ( ticksToSleep > 0 )
		{
			gTicksAtLastTrueSleep = ::LMGetTicks();
		}
		
		return nextEvent;
	}
	
	static void EventLoop()
	{
		while ( ! gEndOfEventLoop  ||  (CONFIG_ADB  &&  gKeyboardConfigured) )
		{
			try
			{
				if ( CONFIG_ADB )
				{
					CheckKeyboard();
				}
				
				ThreadYield();
				
				if ( !ActivelyBusy() || ReadyToWaitForEvents() )
				{
					using mac::app::quitting;
					
					EventRecord event = GetAnEvent();
					
					gEventCheckNeeded = false;
					
					
					gTickCountAtLastContextSwitch = ::LMGetTicks();
					
					CheckShiftSpaceQuasiMode( event );
					
					(void) DispatchCursor( event );
					
					gIdleNeeded = false;
					
					if ( event.what != nullEvent )
					{
						DispatchEvent( event );
						
						gEventCheckNeeded = true;
						
						gIdleNeeded = true;
					}
					else if ( (gIdleNeeded = quitting)  &&  ReadyToExit() )
					{
						gEndOfEventLoop = true;
						
						if ( CONFIG_ADB )
						{
							gNeedToConfigureKeyboard = gKeyboardConfigured;
						}
					}
					else
					{
						GiveIdleTimeToWindows( event );
					}
				}
			}
			catch ( ... )
			{
				DebugBeep();
			}
		}
	}
	
	int Application::Run()
	{
		using mac::sys::current_process;
		using mac::sys::is_front_process;
		
		gInForeground = is_front_process( current_process() );
		
		if ( CONFIG_ADB )
		{
			gNeedToConfigureKeyboard = gInForeground;
		}
		
		SetEventMask( everyEvent );
		
		EventLoop();
		
		return 0;
	}
	
	struct UnhighlightMenus
	{
		void operator()() const  { N::HiliteMenu(); }
	};
	
	template < class Func >
	class AtEnd
	{
		public:
			AtEnd( const Func& func = Func() ) : func( func )  {}
			~AtEnd()                                           { func(); }
		
		private:
			Func func;
	};
	
	void HandleMenuChoice( long menuChoice )
	{
		N::MenuID menuID = N::MenuID( HiWord( menuChoice ) );
		SInt16    item   =            LoWord( menuChoice );
		
		AtEnd< UnhighlightMenus > unhighlightMenus;
		
		if ( CommandCode code = HandleMenuItem( menuID, item ) )
		{
			DispatchMenuItem( code );
		}
		else if ( menuID == N::GetMenuID( the_Window_menu ) )
		{
			WindowRef w = get_nth_window( item - 1 );
			
			SelectWindow( w );
		}
	}
	
	bool DoCommand( CommandCode code )
	{
		if ( CommandHandler handler = GetCommandHandler( code ) )
		{
			return handler( code );
		}
		
		switch ( code )
		{
			case 'clos':
				if ( WindowRef window = N::FrontWindow() )
				{
					close_window( window );
				}
				break;
			
			case 'quit':
				if ( apple_events_present )
				{
					// Direct dispatch
					N::AESend( kCoreEventClass, kAEQuitApplication );
				}
				else
				{
					Quit();
				}
				break;
			
			default:
				return false;
				break;
		}
		
		return true;
	}
	
	void AdjustSleepForTimer( unsigned ticksToSleep )
	{
		const UInt32 businessTime = ::LMGetTicks() + ticksToSleep;
		
		if ( businessTime < gTicksAtNextBusiness )
		{
			gTicksAtNextBusiness = businessTime;
		}
	}
	
	void ScheduleImmediateEventCheck()
	{
		gEventCheckNeeded = true;
	}
	
	void Quit()
	{
		WindowRef window = FrontWindow();
		
		while ( window != NULL )
		{
			WindowRef next = GetNextWindow( window );
			
			close_window( window );
			
			window = next;
		}
		
		mac::app::quitting = true;
	}
	
}
