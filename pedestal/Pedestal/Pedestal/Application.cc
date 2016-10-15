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

// mac-sys-utils
#include "mac_sys/async_wakeup.hh"
#include "mac_sys/current_process.hh"
#include "mac_sys/gestalt.hh"
#include "mac_sys/is_front_process.hh"

// Debug
#include "debug/assert.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

#include "Nitrogen/AEInteraction.hh"
#include "Nitrogen/AppleEvents.hh"
#include "Nitrogen/Controls.hh"
#include "Nitrogen/Events.hh"
#include "Nitrogen/MacErrors.hh"
#include "Nitrogen/MacWindows.hh"
#include "Nitrogen/Menus.hh"
#include "Nitrogen/Quickdraw.hh"
#include "Nitrogen/Resources.hh"
#include "Nitrogen/Threads.hh"

// MacFeatures
#include "MacFeatures/Threads.hh"

#if !TARGET_API_MAC_CARBON

// Arcana
#include "ADB/KeyboardLEDs.hh"
#include "ADB/KeyboardModifiers.hh"

#endif

// Pedestal
#include "Pedestal/ClickTarget.hh"
#include "Pedestal/Commands.hh"
#include "Pedestal/Initialize.hh"
#include "Pedestal/MenuBar.hh"
#include "Pedestal/TrackControl.hh"
#include "Pedestal/Quasimode.hh"
#include "Pedestal/Window.hh"
#include "Pedestal/WindowMenu.hh"


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
	
	const uint32_t gestaltMenuMgrAttr   = 'menu';
	const uint32_t gestaltSystemVersion = 'sysv';
	
	const int gestaltMenuMgrAquaLayoutBit = 1;
	
	static const UInt32 kEitherShiftKey   = shiftKey   | rightShiftKey;
	static const UInt32 kEitherOptionKey  = optionKey  | rightOptionKey;
	static const UInt32 kEitherControlKey = controlKey | rightControlKey;
	
	static MenuRef the_Window_menu;
	
	
	const uint32_t sysv = mac::sys::gestalt( gestaltSystemVersion );
	
	const bool apple_events_present = ! TARGET_CPU_68K  ||  sysv >= 0x0700;
	
	
	struct AppleEventSignature
	{
		Nitrogen::AEEventClass eventClass;
		Nitrogen::AEEventID    eventID;
		
		AppleEventSignature()  {}
		
		AppleEventSignature( Nitrogen::AEEventClass  eventClass,
		                     Nitrogen::AEEventID     eventID )
		:
			eventClass( eventClass ),
			eventID   ( eventID    )
		{}
	};
	
	struct RunState
	{
		AppleEventSignature signatureOfFirstAppleEvent;
		
		bool inForeground;     // set to true when the app is frontmost
		bool startupComplete;  // set to true once the app is ready to respond to events
		bool quitRequested;    // set to true when quitting is in process, to false if cancelled
		bool endOfEventLoop;   // set to true once the app is ready to stop processing events
		
		RunState()
		:
			inForeground   ( false ),  // we have to check
			startupComplete( false ),
			quitRequested  ( false ),
			endOfEventLoop ( false )
		{}
	};
	
	
	static RunState gRunState;
	
	static UInt32 gTickCountAtLastContextSwitch = 0;
	
	static Point gLastMouseLocation;
	
#if !TARGET_API_MAC_CARBON
	
	// ADB address of the keyboard from the last key-down event.
	static N::ADBAddress gLastKeyboard;
	
#endif
	
	static bool gKeyboardConfigured      = false;
	static bool gNeedToConfigureKeyboard = false;
	
	static bool gShiftKeyIsDownFromKeyStroke = false;
	
	static UInt32 gShiftSpaceQuasimodeMask = 0;
	
	static boost::intrusive_ptr< Quasimode > gQuasimode;
	
	
	static inline void DebugBeep()
	{
		Mac::SysBeep();
	}
	
	enum
	{
		idAppleMENU = 128,  // menu ID = 1
		idFileMENU,
		idEditMENU,
		idWindowMENU,
		//, idDebugMENU = 255  // menu ID = 128
	};
	
	
	bool (*gActivelyBusy_Hook)() = NULL;
	bool (*gReadyToExit_Hook )() = NULL;
	
	static bool ActivelyBusy()
	{
		return gActivelyBusy_Hook ? gActivelyBusy_Hook() : false;
	}
	
	static bool ReadyToExit()
	{
		return gReadyToExit_Hook ? gReadyToExit_Hook() : true;
	}
	
	static bool DoCommand( CommandCode code );
	
	static bool DispatchMenuItem( CommandCode code )
	{
		bool handled = false;
		
		if ( View* view = get_window_view( FrontWindow() ) )
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
	
#if !TARGET_API_MAC_CARBON
	
	static inline N::ADBAddress GetKeyboardFromEvent( const EventRecord& event )
	{
		return N::ADBAddress( (event.message & adbAddrMask) >> 16 );
	}
	
	static void ConfigureKeyboard( N::ADBAddress keyboard, bool active )
	{
		UInt8 capsLED = ::GetCurrentKeyModifiers() & alphaLock ? 2 : 0;
		
		SetLEDs( keyboard, (active ? 1 : 0) | capsLED );
		
		SetKeyboardModifiersDistinctness( keyboard, active );
		
		gKeyboardConfigured = active;
	}
	
#endif
	
	static
	void SuspendResume( SuspendResume_flag flag )
	{
		gNeedToConfigureKeyboard = true;
		
		if ( WindowRef front = FrontWindow() )
		{
			window_activated( front, flag != Flag_suspending );
		}
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
	
	static void RespondToDrag( const EventRecord& event, WindowRef window )
	{
		Rect before = N::GetPortBounds( N::GetWindowPort( window ) );
		
		N::DragWindow( window, event.where, N::GetQDGlobalsScreenBits().bounds );
		
		// FIXME
		if ( false )
		{
			Rect after = N::GetPortBounds( N::GetWindowPort( window ) );
			
			if ( before.top != after.top  ||  before.left != after.left )
			{
				//window->Moved( after );
			}
		}
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
	
	static void RespondToContent( const EventRecord& event, WindowRef windowRef )
	{
		Point pt = N::GlobalToLocal( event.where );
		
		// TrackedControl's result indicates whether a control was found.
		if ( TrackedControl( N::FindControl( pt, windowRef ), pt ) )
		{
			// already handled
		}
		else
		{
			window_mouseDown( windowRef, event );
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
		if ( View* view = get_window_view( FrontWindow() ) )
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
		
	#if !TARGET_API_MAC_CARBON
		
		gLastKeyboard = GetKeyboardFromEvent( event );
		
	#endif
		
		if ( event.what == keyUp )
		{
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
		WindowRef windowRef = reinterpret_cast< ::WindowRef >( event.message );
		
		ASSERT( windowRef != NULL );
		
		N::Update_Scope update( windowRef );
		
		if ( ::IsPortVisibleRegionEmpty( N::GetWindowPort( windowRef ) ) )
		{
			return;
		}
		
		{
			SetPortWindowPort( windowRef );
			
			window_update( windowRef );
			
			n::saved< N::Clip > savedClip;
			
			N::ClipRect( N::GetPortBounds( N::GetWindowPort( windowRef ) ) );
			
			N::UpdateControls( windowRef );
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
			err = ::DIBadMount( Point(), message );  // System 7 ignores the point
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
					
					gRunState.inForeground = flag != Flag_suspending;
					
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
			case kHighLevelEvent:  DispatchHighLevelEvent( event );  break;
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
	
	struct AppleEvent
	{
		
		static void Handler( const Mac::AppleEvent&  appleEvent,
		                     Mac::AppleEvent&        reply );
		
	};
	
	static MenuRef GetAndInsertMenu( N::ResID resID )
	{
		MenuRef menu = N::GetMenu( resID );
		
		::InsertMenu( menu, MenuID() );
		
		return menu;
	}
	
	Application::Application()
	{
		Init_MacToolbox();
		
		Init_Memory( 0 );
		
		if ( apple_events_present )
		{
			N::AEInstallEventHandler< AppleEvent::Handler >( kCoreEventClass,
			                                                 N::AEEventID( typeWildCard ) ).release();
		}
		
		MenuRef appleMenu = GetAndInsertMenu( N::ResID( idAppleMENU ) );
		MenuRef fileMenu  = GetAndInsertMenu( N::ResID( idFileMENU  ) );
		MenuRef editMenu  = GetAndInsertMenu( N::ResID( idEditMENU  ) );
		MenuRef windowMenu = GetAndInsertMenu( N::ResID( idWindowMENU ) );
		
		the_Window_menu = windowMenu;
		
		if ( TARGET_API_MAC_CARBON )
		{
			const int aquaMenusBit = gestaltMenuMgrAquaLayoutBit;
			
			if ( mac::sys::gestalt_bit_set( gestaltMenuMgrAttr, aquaMenusBit ) )
			{
				SInt16 last = CountMenuItems( fileMenu );
				
				// Delete "Quit" and the separator above it.
				
				DeleteMenuItem( fileMenu, last     );
				DeleteMenuItem( fileMenu, last - 1 );
			}
		}
		
		AddMenu( appleMenu );
		AddMenu( fileMenu  );
		AddMenu( editMenu  );
		AddMenu( windowMenu );
		
		if ( !TARGET_API_MAC_CARBON )
		{
			PopulateAppleMenu( appleMenu );
		}
		
		DrawMenuBar();
	}
	
	
	static void CheckMouse()
	{
		using namespace nucleus::operators;
		
		Point mouseLocation = N::GetGlobalMouse();
		
		if ( mouseLocation != gLastMouseLocation )
		{
			gLastMouseLocation = mouseLocation;
		}
	}
	
	static void CheckKeyboard()
	{
	#if !TARGET_API_MAC_CARBON
		
		if ( gNeedToConfigureKeyboard  &&  gLastKeyboard != 0 )
		{
			// Don't reconfigure the keyboard if certain modifiers are down,
			// since that confuses the OS
			UInt32 confusingModifiers =   kEitherShiftKey
			                            | kEitherOptionKey
			                            | kEitherControlKey;
			
			if ( (::GetCurrentKeyModifiers() & confusingModifiers) == 0 )
			{
				bool active = gRunState.inForeground && !gRunState.endOfEventLoop;
				
				ConfigureKeyboard( gLastKeyboard, active );
				
				gNeedToConfigureKeyboard = false;
			}
		}
		
	#endif
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
	
	static void set_pixel_region( RgnHandle rgn, Point pt )
	{
		Rect rect = { pt.v, pt.h, pt.v + 1, pt.h + 1 };
		
		RectRgn( rgn, &rect );
	}
	
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
		
		set_pixel_region( current_mouse_location, last_global_mouse );
		
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
	
	static const bool has_ThreadManager = MacFeatures::Has_Threads();
	
	static void EventLoop()
	{
		// Use two levels of looping.
		// This lets us loop inside the try block without entering and leaving,
		// and will continue looping if an exception is thrown.
		while ( !gRunState.endOfEventLoop || gKeyboardConfigured )
		{
			try
			{
				while ( !gRunState.endOfEventLoop || gKeyboardConfigured )
				{
					CheckMouse();
					
					CheckKeyboard();
					
					if ( has_ThreadManager )
					{
						N::YieldToAnyThread();
					}
					
					if ( !ActivelyBusy() || ReadyToWaitForEvents() )
					{
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
						else if ( (gIdleNeeded = gRunState.quitRequested)  &&  ReadyToExit() )
						{
							gRunState.endOfEventLoop = true;
							
							gNeedToConfigureKeyboard = gKeyboardConfigured;
						}
						else
						{
							GiveIdleTimeToWindows( event );
						}
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
		
		gRunState.inForeground = is_front_process( current_process() );
		
		gNeedToConfigureKeyboard = gRunState.inForeground;
		
		SetEventMask( everyEvent );
		
		EventLoop();
		
		return 0;
	}
	
	void AppleEvent::Handler( const Mac::AppleEvent& appleEvent, Mac::AppleEvent& reply )
	{
		Mac::AEEventClass eventClass = N::AEGetAttributePtr< Mac::keyEventClassAttr >( appleEvent );
		Mac::AEEventID    eventID    = N::AEGetAttributePtr< Mac::keyEventIDAttr    >( appleEvent );
		
		static bool firstTime = true;
		
		if ( firstTime )
		{
			gRunState.signatureOfFirstAppleEvent = AppleEventSignature( eventClass, eventID );
			firstTime = false;
		}
		
		if ( eventClass == kCoreEventClass )
		{
			switch ( eventID )
			{
				case kAEOpenApplication:
					//myOpenAppReceived = true;
					break;
				
				case kAEQuitApplication:
					gRunState.quitRequested = true;
					break;
				
				default:
					throw N::ErrAEEventNotHandled();
					break;
			}
		}
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
					gRunState.quitRequested = true;
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
		gRunState.quitRequested = true;
	}
	
}
