/*	==============
 *	Application.cc
 *	==============
 */

#include "Pedestal/Application.hh"

// Mac OS
#ifndef __MACH__
	#include <Controls.h>
	#include <DiskInit.h>
	#include <ToolUtils.h>
#endif

// Nostalgia
#include "Nostalgia/LowMem.hh"

// Debug
#include "debug/assert.hh"

// Nucleus
#include "Nucleus/Saved.h"

// Nitrogen
#include "Nitrogen/AEInteraction.h"
#include "Nitrogen/Events.h"
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/Sound.h"
#include "Nitrogen/Threads.h"

#if !TARGET_API_MAC_CARBON

// Arcana
#include "ADB/KeyboardLEDs.hh"
#include "ADB/KeyboardModifiers.hh"

#endif

// Pedestal
#include "Pedestal/ApplicationContext.hh"
#include "Pedestal/TrackControl.hh"
#include "Pedestal/Quasimode.hh"
#include "Pedestal/Window.hh"


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
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	// Save our PSN so we can wake up at interrupt time.
	static N::ProcessSerialNumber gPSN = N::GetCurrentProcess();
	
	
	using N::kCoreEventClass;
	using N::kAEQuitApplication;
	
	static const UInt32 kEitherShiftKey   = shiftKey   | rightShiftKey;
	static const UInt32 kEitherOptionKey  = optionKey  | rightOptionKey;
	static const UInt32 kEitherControlKey = controlKey | rightControlKey;
	
	struct RunState
	{
		AppleEventSignature signatureOfFirstAppleEvent;
		
		bool inForeground;     // set to true when the app is frontmost
		bool startupComplete;  // set to true once the app is ready to respond to events
		bool activelyBusy;     // set to true by active threads, reset in event loop
		bool quitRequested;    // set to true when quitting is in process, to false if cancelled
		bool endOfEventLoop;   // set to true once the app is ready to stop processing events
		
		RunState()
		:
			inForeground   ( false ),  // we have to check
			startupComplete( false ),
			activelyBusy   ( false ),
			quitRequested  ( false ),
			endOfEventLoop ( false )
		{}
	};
	
	
	static RunState gRunState;
	
	static UInt32 gTickCountAtLastContextSwitch = 0;
	static UInt32 gTickCountAtLastUserEvent     = 0;
	
	static Point gLastMouseLocation;
	
#if !TARGET_API_MAC_CARBON
	
	// ADB address of the keyboard from the last key-down event.
	static N::ADBAddress gLastKeyboard;
	
#endif
	
	static bool gKeyboardConfigured      = false;
	static bool gNeedToConfigureKeyboard = false;
	
	static bool gShiftKeyIsDownFromKeyStroke = false;
	
	static UInt32 gShiftSpaceQuasimodeMask = 0;
	
	static boost::shared_ptr< Quasimode > gQuasimode;
	
	
	inline void DebugBeep()
	{
		N::SysBeep();
	}
	
	enum
	{
		idAppleMENU = 128,  // menu ID = 1
		idFileMENU,
		idEditMENU
		//, idDebugMENU = 255  // menu ID = 128
	};
	
	static Application* gApp = NULL;
	
	Application& TheApp()
	{
		return *gApp;
	}
	
	static void UpdateLastUserEvent()
	{
		gTickCountAtLastUserEvent = ::LMGetTicks();
	}
	
	bool MenuItemDispatcher::Run( MenuItemCode code ) const
	{
		bool handled = false;
		
		if ( N::WindowRef window = N::FrontWindow() )
		{
			if ( N::GetWindowKind( window ) == N::kApplicationWindowKind )
			{
				if ( Window* base = N::GetWRefCon( window ) )
				{
					N::SetPortWindowPort( window );
					
					handled = base->GetView()->UserCommand( code );
				}
			}
		}
		
		handled = handled || app.DoCommand( code );
		
		if ( !handled )
		{
			DebugBeep();  // FIXME
		}
		
		return handled;
	}
	
#if !TARGET_API_MAC_CARBON
	
	inline N::ADBAddress GetKeyboardFromEvent( const EventRecord& event )
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
	
	static void Suspend()
	{
		gNeedToConfigureKeyboard = true;
		
		N::WindowRef window = N::FrontWindow();
		
		if ( window  &&  N::GetWindowKind( window ) == N::kApplicationWindowKind )
		{
			if ( Window* base = N::GetWRefCon( window ) )
			{
				N::SetPortWindowPort( window );
				
				return base->Activate( false );
			}
		}
	}
	
	static void Resume()
	{
		gNeedToConfigureKeyboard = true;
		
		N::WindowRef window = N::FrontWindow();
		
		if ( window  &&  N::GetWindowKind( window ) == N::kApplicationWindowKind )
		{
			if ( Window* base = N::GetWRefCon( window ) )
			{
				N::SetPortWindowPort( window );
				
				return base->Activate( true );
			}
		}
	}
	
	
	/*
	 *	--------------------------
	 *	Event processing routines.
	 *	--------------------------
	 */
	
	static bool DispatchCursorToFrontWindow( const EventRecord& event )
	{
		N::WindowRef window = N::FrontWindow();
		
		if ( window  &&  N::GetWindowKind( window ) == N::kApplicationWindowKind )
		{
			if ( Window* base = N::GetWRefCon( window ) )
			{
				N::SetPortWindowPort( window );
				
				return base->GetView()->SetCursor( event, NULL );
			}
		}
		
		return false;
	}
	
	static bool DispatchCursor( const EventRecord& event )
	{
		if ( !DispatchCursorToFrontWindow( event ) )
		{
			N::SetCursor( N::GetQDGlobalsArrow() );
		}
		
		return true;
	}
	
	static void DispatchHighLevelEvent( const EventRecord& event )
	{
		ASSERT( event.what == kHighLevelEvent );
		
		try
		{
			N::AEProcessAppleEvent( event );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err != errAEEventNotHandled )
			{
				throw;
			}
		}
	}
	
	static void RespondToDrag( const EventRecord& event, N::WindowRef window )
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
	
	static void RespondToContent( const EventRecord& event, N::WindowRef window )
	{
		Point pt = N::GlobalToLocal( event.where );
		
		// TrackedControl's result indicates whether a control was found.
		if ( TrackedControl( N::FindControl( pt, window ), pt ) )
		{
			// already handled
		}
		else if ( N::GetWindowKind( window ) == N::kApplicationWindowKind )
		{
			if ( Window* base = N::GetWRefCon( window ) )
			{
				base->MouseDown( event );
			}
		}
	}
	
	static void RespondToGrow( const EventRecord& event, N::WindowRef window )
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
	
	static void RespondToGoAway( const EventRecord& event, N::WindowRef window )
	{
		if ( N::TrackGoAway( window, event.where ) )
		{
			if ( Window* base = N::GetWRefCon( window ) )
			{
				base->Close( window );
			}
		}
	}
	
	static void DispatchMouseDown( const EventRecord& event )
	{
		ASSERT( event.what == mouseDown );
		
		UpdateLastUserEvent();
		
		N::FindWindow_Result found = N::FindWindow( event.where );
		
		if ( found.part == N::inMenuBar )
		{
			TheApp().HandleMenuChoice( ::MenuSelect( event.where ) );
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
		
		UpdateLastUserEvent();
	}
	
	inline bool CharIsArrowKey( char c )
	{
		return (c & 0xFC) == 0x1C;
	}
	
	inline bool CharIsDelete( char c )
	{
		return c == kBackspaceCharCode  ||  c == kDeleteCharCode;
	}
	
	inline bool CharMayBeCommand( char c )
	{
		// Command-Space is passed as a key-down, never a menu command.
		// Command-arrow is an editing gesture, not a command.
		// Command-Delete is also an editing gesture.
		
		return c != ' '  &&  !CharIsArrowKey( c )  &&  !CharIsDelete( c );
	}
	
	inline bool ShouldEnterShiftSpaceQuasiMode( const EventRecord& event )
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
		if ( N::WindowRef window = N::FrontWindow() )
		{
			if ( N::GetWindowKind( window ) == N::kApplicationWindowKind )
			{
				if ( Window* base = N::GetWRefCon( window ) )
				{
					N::SetPortWindowPort( window );
					
					if ( gQuasimode = base->GetView()->EnterShiftSpaceQuasimode( event ) )
					{
						gShiftSpaceQuasimodeMask = event.modifiers & kEitherShiftKey;
						
						return;
					}
				}
			}
		}
		
		N::SysBeep();
	}
	
	static void DispatchKey( const EventRecord& event )
	{
		ASSERT( event.what == keyDown || event.what == autoKey );
		
		UpdateLastUserEvent();
		
	#if !TARGET_API_MAC_CARBON
		
		gLastKeyboard = GetKeyboardFromEvent( event );
		
	#endif
		
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
			TheApp().HandleMenuChoice( ::MenuKey( c ) );
		}
		else if ( gQuasimode && gQuasimode->KeyDown( event ) )
		{
			// done
		}
		else if ( ShouldEnterShiftSpaceQuasiMode( event ) )
		{
			EnterShiftSpaceQuasimode( event );
		}
		else if ( N::WindowRef window = N::FrontWindow() )
		{
			if ( N::GetWindowKind( window ) == N::kApplicationWindowKind )
			{
				if ( Window* base = N::GetWRefCon( window ) )
				{
					N::SetPortWindowPort( window );
					
					base->GetView()->KeyDown( event );
				}
			}
		}
		
		gShiftKeyIsDownFromKeyStroke = event.modifiers & kEitherShiftKey;
	}
	
	static void DispatchActivate( const EventRecord& event )
	{
		N::WindowRef window = reinterpret_cast< ::WindowRef >( event.message );
		
		ASSERT( window != NULL );
		
		N::SetPortWindowPort( window );
		
		if ( N::GetWindowKind( window ) == N::kApplicationWindowKind )
		{
			if ( Window* base = N::GetWRefCon( window ) )
			{
				base->Activate( event.modifiers & activeFlag );
			}
		}
	}
	
	static void DispatchUpdate( const EventRecord& event )
	{
		N::WindowRef window = reinterpret_cast< ::WindowRef >( event.message );
		
		ASSERT( window != NULL );
		
		N::SetPortWindowPort( window );
		
		N::Update_Scope update( window );
		
		if ( ::IsPortVisibleRegionEmpty( N::GetWindowPort( window ) ) )
		{
			return;
		}
		
		if ( N::GetWindowKind( window ) == N::kApplicationWindowKind )
		{
			if ( Window* base = N::GetWRefCon( window ) )
			{
				base->Update();
			}
		}
		
		NN::Saved< N::Clip_Value > savedClip;
		
		N::ClipRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
		
		N::UpdateControls( window );
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
				gRunState.inForeground = event.message & resumeFlag;
				
				if ( gRunState.inForeground )
				{
					Resume();
				}
				else
				{
					Suspend();
				}
				break;
			
			case mouseMovedMessage:
				break;
			
			default:
				break;
		}
	}
	
	
	void DispatchEvent( const EventRecord& event )
	{
		switch ( event.what )
		{
		//	case nullEvent:        DispatchNullEvent     ( event );  break;
			case kHighLevelEvent:  DispatchHighLevelEvent( event );  break;
			case mouseDown:        DispatchMouseDown     ( event );  break;
			case keyDown:
				case autoKey:      DispatchKey           ( event );  break;
			case activateEvt:      DispatchActivate      ( event );  break;
			case updateEvt:        DispatchUpdate        ( event );  break;
			case diskEvt:          DispatchDiskInsert    ( event );  break;
			case osEvt:            DispatchOSEvent       ( event );  break;
			
			case mouseUp:
				UpdateLastUserEvent();
				break;
			
			default:
				break;
		}
	}
	
	static void GiveIdleTimeToWindow( N::WindowRef window, const EventRecord& event )
	{
		if ( N::GetWindowKind( window ) == N::kApplicationWindowKind )
		{
			if ( Window* base = N::GetWRefCon( window ) )
			{
				base->GetView()->Idle( event );
			}
		}
	}
	
	static void GiveIdleTimeToWindows( const EventRecord& event )
	{
		NN::Saved< N::Port_Value > savePort;
		
		// FIXME:  Use window iterator
		for ( N::WindowRef window = N::FrontWindow();
		      window != NULL;
		      //window = N::GetNextWindow( window ) )  // FIXME
		      window = ::GetNextWindow( window ) )
		{
			N::SetPortWindowPort( window );
			
			GiveIdleTimeToWindow( window, event );
		}
	}
	
	void Application::AppleEventHandler( const N::AppleEvent&  appleEvent,
	                                     N::AppleEvent&        reply,
	                                     Application*          app )
	{
		app->HandleAppleEvent( appleEvent, reply );
	}
	
	void Application::RegisterMenuItemHandler( MenuItemCode code, MenuItemHandler* handler )
	{
		menuItemHandlers[ code ] = handler;
	}
	
	Application::Application()
	:
		menuItemDispatcher( *this ),
		myMenubar  ( menuItemDispatcher ),
		myAppleMenu( N::InsertMenu( N::GetMenu( N::ResID( idAppleMENU ) ) ) ),
		myFileMenu ( N::InsertMenu( N::GetMenu( N::ResID( idFileMENU  ) ) ) ),
		myEditMenu ( N::InsertMenu( N::GetMenu( N::ResID( idEditMENU  ) ) ) ),
		myCoreEventsHandler( N::AEInstallEventHandler< Application*,
		                                               AppleEventHandler >( kCoreEventClass,
		                                                                    N::AEEventID( typeWildCard ),
		                                                                    this ) )
	{
		ASSERT( gApp == NULL );
		gApp = this;
		
		if ( N::Gestalt_Mask< N::gestaltMenuMgrAttr, gestaltMenuMgrAquaLayoutMask >() )
		{
			MenuRef fileMenu = N::GetMenuRef( myFileMenu );
			SInt16 last = N::CountMenuItems( fileMenu );
			N::DeleteMenuItem( fileMenu, last );
			N::DeleteMenuItem( fileMenu, last - 1 );  // Quit item has a separator above it
		}
		
		myMenubar.AddAppleMenu( myAppleMenu );
		myMenubar.AddMenu     ( myFileMenu  );
		myMenubar.AddMenu     ( myEditMenu  );
		
		N::InvalMenuBar();
	}
	
	Application::~Application()
	{
	}
	
	static void CheckMouse()
	{
		using namespace Nucleus::Operators;
		
		Point mouseLocation = N::GetMouse();
		
		if ( mouseLocation != gLastMouseLocation )
		{
			gLastMouseLocation = mouseLocation;
			
			UpdateLastUserEvent();
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
	
	static EventRecord GetAnEvent()
	{
		const UInt32 now = ::LMGetTicks();
		
		UInt32 ticksToSleep = 0x7FFFFFFF;
		
		// If we're actively busy (i.e. some thread is in Breathe()), sleep for
		// at most one tick, and that only if it's been a while since.
		
		if ( gRunState.activelyBusy )
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
		
		EventRecord nextEvent = N::WaitNextEvent( N::everyEvent, ticksToSleep );
		
		if ( ticksToSleep > 0 )
		{
			gTicksAtLastTrueSleep = ::LMGetTicks();
		}
		
		return nextEvent;
	}
	
	void Application::EventLoop()
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
					gRunState.activelyBusy = false;
					
					CheckMouse();
					
					CheckKeyboard();
					
					N::YieldToAnyThread();
					
					if ( !gRunState.activelyBusy || ReadyToWaitForEvents() )
					{
						EventRecord event = GetAnEvent();
						
						gEventCheckNeeded = false;
						
						
						gTickCountAtLastContextSwitch = ::LMGetTicks();
						
						CheckShiftSpaceQuasiMode( event );
						
						(void) DispatchCursor( event );
						
						if ( event.what != nullEvent )
						{
							DispatchEvent( event );
							
							gEventCheckNeeded = true;
							
							gIdleNeeded = true;
						}
						else if ( gRunState.quitRequested )
						{
							gRunState.endOfEventLoop = true;
							
							gNeedToConfigureKeyboard = gKeyboardConfigured;
						}
						else
						{
							GiveIdleTimeToWindows( event );
							
							gIdleNeeded = false;
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
		gRunState.inForeground = N::SameProcess( N::GetFrontProcess(), N::CurrentProcess() );
		
		gNeedToConfigureKeyboard = gRunState.inForeground;
		
		EventLoop();
		
		return 0;
	}
	
	void Application::HandleAppleEvent( const N::AppleEvent& appleEvent, N::AppleEvent& reply )
	{
		N::AEEventClass eventClass = N::AEGetAttributePtr< N::keyEventClassAttr >( appleEvent );
		N::AEEventID    eventID    = N::AEGetAttributePtr< N::keyEventIDAttr    >( appleEvent );
		
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
	
	void Application::HandleMenuChoice( long menuChoice )
	{
		myMenubar.ProcessMenuItem( menuChoice );
	}
	
	bool Application::DoCommand( MenuItemCode code )
	{
		typedef MenuItemHandlerMap::const_iterator const_iterator;
		
		const_iterator found = menuItemHandlers.find( code );
		
		if ( found != menuItemHandlers.end() )
		{
			MenuItemHandler* handler = found->second;
			
			return handler->Run( code );
		}
		
		switch ( code )
		{
			case 'clos':
				if ( N::WindowRef window = N::FrontWindow() )
				{
					if ( Window* base = N::GetWRefCon( window ) )
					{
						base->Close( window );
					}
				}
				break;
			
			case 'quit':
				// Direct dispatch
				N::AESend( kCoreEventClass, kAEQuitApplication );
				break;
			
			default:
				return false;
				break;
		}
		
		return true;
	}
	
	void WakeUp()
	{
		ProcessSerialNumber psn;
		
		OSErr err = ::GetCurrentProcess( &psn );
		
		if ( err == noErr )
		{
			Boolean same;
			
			err = ::SameProcess( &psn, &gPSN, &same );
			
			if ( err == noErr  &&  same )
			{
				gRunState.activelyBusy = true;
				
				return;
			}
		}
		
		::WakeUpProcess( &gPSN );
	}
	
	void AdjustSleepForTimer( UInt32 ticksToSleep )
	{
		const UInt32 businessTime = ::LMGetTicks() + ticksToSleep;
		
		if ( businessTime < gTicksAtNextBusiness )
		{
			gTicksAtNextBusiness = businessTime;
		}
	}
	
	void AdjustSleepForActivity()
	{
		gRunState.activelyBusy = true;
	}
	
	void ScheduleImmediateEventCheck()
	{
		gEventCheckNeeded = true;
	}
	
}

