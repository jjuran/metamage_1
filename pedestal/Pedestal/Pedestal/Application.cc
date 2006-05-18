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

// Nitrogen Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen / Carbon support
#include "Nitrogen/AEInteraction.h"
#include "Nitrogen/Events.h"
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/Sound.h"
#include "Nitrogen/Threads.h"

// Nitrogen Extras / Utilities
#include "Utilities/Clipboard.h"
#include "Utilities/Saved.h"

// Pedestal
#include "Pedestal/ApplicationContext.hh"
#include "Pedestal/Control.hh"
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
	namespace NX = NitrogenExtras;
	
	struct RunState
	{
		AppleEventSignature signatureOfFirstAppleEvent;
		UInt32 tickCountAtLastLayerSwitch;
		UInt32 maxTicksToSleep;
		
		bool inForeground;     // set to true when the app is frontmost
		bool startupComplete;  // set to true once the app is ready to respond to events
		bool activelyBusy;     // set to true by active threads, reset in event loop
		bool quitRequested;    // set to true when quitting is in process, to false if cancelled
		bool endOfEventLoop;   // set to true once the app is ready to stop processing events
		
		RunState()
		:
			tickCountAtLastLayerSwitch( 0 ),
			maxTicksToSleep           ( 0 ),
			inForeground   ( false ),  // we have to check
			startupComplete( false ),
			activelyBusy   ( false ),
			quitRequested  ( false ),
			endOfEventLoop ( false )
		{}
	};
	
	
	RunState gRunState;
	
	
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
	
	bool MenuItemDispatcher::Run( MenuItemCode code ) const
	{
		bool handled = false;
		
		if ( N::WindowRef window = N::FrontWindow() )
		{
			if ( N::GetWindowKind( window ) == kApplicationWindowKind )
			{
				if ( WindowBase* base = N::GetWRefCon( window ) )
				{
					handled = base->UserCommand( code );
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
	
	static void Suspend()
	{
		N::WindowRef window = N::FrontWindow();
		
		if ( window  &&  N::GetWindowKind( window ) == kApplicationWindowKind )
		{
			if ( WindowBase* base = N::GetWRefCon( window ) )
			{
				N::SetPortWindowPort( window );
				return base->Activate( false );
			}
		}
	}
	
	static void Resume()
	{
		N::WindowRef window = N::FrontWindow();
		
		if ( window  &&  N::GetWindowKind( window ) == kApplicationWindowKind )
		{
			if ( WindowBase* base = N::GetWRefCon( window ) )
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
		
		if ( window  &&  N::GetWindowKind( window ) == kApplicationWindowKind )
		{
			if ( WindowBase* base = N::GetWRefCon( window ) )
			{
				N::SetPortWindowPort( window );
				return base->SetCursor( N::GlobalToLocal( event.where ), NULL );
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
		catch ( N::ErrAEEventNotHandled )
		{
			//
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
	
	static void RespondToContent( const EventRecord& event, N::WindowRef window )
	{
		Point pt = N::GlobalToLocal( event.where );
		
		// TrackControl's result indicates whether a control was found.
		if ( TrackControl( N::FindControl( pt, window ), pt ) )
		{
			
		}
		else if ( N::GetWindowKind( window ) == kApplicationWindowKind )
		{
			if ( WindowBase* base = N::GetWRefCon( window ) )
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
			if ( N::GetWindowKind( window ) == kApplicationWindowKind )
			{
				if ( WindowBase* base = N::GetWRefCon( window ) )
				{
					// Resize the window
					N::SizeWindow( window, grown.h, grown.v, true );
					
					base->Resized( N::GetPortBounds( N::GetWindowPort( window ) ) );
				}
			}
		}
	}
	
	static void RespondToGoAway( const EventRecord& event, N::WindowRef window )
	{
		if ( N::TrackGoAway( window, event.where ) )
		{
			if ( WindowBase* base = N::GetWRefCon( window ) )
			{
				base->Closure().RequestWindowClosure( window );
			}
		}
	}
	
	static void DispatchMouseDown( const EventRecord& event )
	{
		ASSERT( event.what == mouseDown );
		
		N::FindWindow_Result found = N::FindWindow( event.where );
		
		if ( found.part.Get() == inMenuBar )
		{
			TheApp().HandleMenuChoice( ::MenuSelect( event.where ) );
			return;
		}
		
		ASSERT( found.window != NULL );
		
	#if CALL_NOT_IN_CARBON
		
		if ( found.part.Get() == inSysWindow )
		{
			::SystemClick( &event, found.window );
			
			return;
		}
		
	#endif
		
		N::SetPortWindowPort( found.window );
		
		switch ( found.part.Get() )
		{
			case inDrag:     RespondToDrag   ( event, found.window );  break;
			case inContent:  RespondToContent( event, found.window );  break;
			case inGrow:     RespondToGrow   ( event, found.window );  break;
			case inGoAway:   RespondToGoAway ( event, found.window );  break;
			
			default:
				break;
		}
	}
	
	static void DispatchKey( const EventRecord& event )
	{
		ASSERT( event.what == keyDown || event.what == autoKey );
		
		if ( event.modifiers & cmdKey )
		{
			// no commands on autoKey
			if ( event.what == keyDown )
			{
				TheApp().HandleMenuChoice( ::MenuKey( event.message & charCodeMask ) );
			}
		}
		else if ( N::WindowRef window = N::FrontWindow() )
		{
			if ( N::GetWindowKind( window ) == kApplicationWindowKind )
			{
				if ( WindowBase* base = N::GetWRefCon( window ) )
				{
					base->KeyDown( event );
				}
			}
		}
	}
	
	static void DispatchActivate( const EventRecord& event )
	{
		N::WindowRef window = reinterpret_cast< ::WindowRef >( event.message );
		
		ASSERT( window != NULL );
		
		N::SetPortWindowPort( window );
		
		if ( N::GetWindowKind( window ) == kApplicationWindowKind )
		{
			if ( WindowBase* base = N::GetWRefCon( window ) )
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
		
		if ( N::GetWindowKind( window ) == kApplicationWindowKind )
		{
			if ( WindowBase* base = N::GetWRefCon( window ) )
			{
				base->Update();
			}
		}
		
		N::Saved< N::Clip_Value > savedClip;
		
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
				if ( gRunState.inForeground = event.message & resumeFlag )
				{
				#if !TARGET_API_MAC_CARBON
					
					if ( event.message & convertClipboardFlag )
					{
						NX::Clipboard::Resume();
					}
					
				#endif
					
					Resume();
				}
				else
				{
					Suspend();
					NX::Clipboard::Suspend();
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
			
			default:
				break;
		}
	}
	
	static void GiveIdleTimeToWindow( N::WindowRef window, const EventRecord& event )
	{
		if ( N::GetWindowKind( window ) == kApplicationWindowKind )
		{
			if ( WindowBase* base = N::GetWRefCon( window ) )
			{
				base->Idle( event );
			}
		}
	}
	
	static void GiveIdleTimeToWindows( const EventRecord& event )
	{
		N::Saved< N::Port_Value > savePort;
		
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
	
	static AEEventClass GetEventClass( const AppleEvent& appleEvent )
	{
		return AEEventClass( N::AEGetAttributePtr< typeType >( appleEvent, N::AEKeyword::Make( keyEventClassAttr ) ) );
	}
	
	static AEEventID GetEventID( const AppleEvent& appleEvent )
	{
		return AEEventID( N::AEGetAttributePtr< typeType >( appleEvent, N::AEKeyword::Make( keyEventIDAttr ) ) );
	}
	
	void Application::AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, Application* app )
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
		myCoreEventsHandler
		(
			N::AEInstallEventHandler< Application*, AppleEventHandler >
			(
				kCoreEventClass,
				typeWildCard,
				this
			)
		)
	{
		ASSERT( gApp == NULL );
		gApp = this;
		
		if ( N::Gestalt_Mask< gestaltMenuMgrAttr, gestaltMenuMgrAquaLayoutMask >() )
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
	
	static bool ReadyToWaitForEvents()
	{
		UInt32 minTicksBetweenWNE = gRunState.inForeground ?  2 :  1;
		
		UInt32 timetoWNE = gRunState.tickCountAtLastLayerSwitch + minTicksBetweenWNE;
		
		UInt32 now = ::TickCount();
		
		bool readyToWait = !gRunState.activelyBusy || now >= timetoWNE;
		
		return readyToWait;
	}
	
	void Application::EventLoop()
	{
		// Use two levels of looping.
		// This lets us loop inside the try block without entering and leaving,
		// and will continue looping if an exception is thrown.
		while ( !gRunState.endOfEventLoop )
		{
			try
			{
				while ( !gRunState.endOfEventLoop )
				{
					gRunState.activelyBusy = false;
					
					Repeaters().PerformAll();
					
					N::YieldToAnyThread();
					
					if ( ReadyToWaitForEvents() )
					{
						EventRecord event = N::WaitNextEvent( everyEvent, gRunState.maxTicksToSleep );
						
						gRunState.tickCountAtLastLayerSwitch = ::TickCount();
						
						(void)DispatchCursor( event );
						
						if ( event.what != nullEvent )
						{
							DispatchEvent( event );
							
							gRunState.maxTicksToSleep = 0;  // Always idle after an event
						}
						else if ( gRunState.quitRequested )
						{
							gRunState.endOfEventLoop = true;
						}
						else
						{
							gRunState.maxTicksToSleep = 0x7FFFFFFF;
							
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
		NX::Clipboard myClipboard;
		
		gRunState.inForeground = N::SameProcess( N::GetFrontProcess(), N::CurrentProcess() );
		
		EventLoop();
		
		return 0;
	}
	
	void Application::HandleAppleEvent( const AppleEvent& appleEvent, AppleEvent& reply )
	{
		AEEventClass eventClass = GetEventClass( appleEvent );
		AEEventID    eventID    = GetEventID   ( appleEvent );
		
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
					if ( WindowBase* base = N::GetWRefCon( window ) )
					{
						base->Closure().RequestWindowClosure( window );
					}
				}
				break;
			
			case 'quit':
				// Direct dispatch
				N::AESend( N::AECreateAppleEvent( kCoreEventClass,
				                                  kAEQuitApplication,
				                                  N::AECreateDesc< typeProcessSerialNumber >( N::CurrentProcess() ) ),
				           kAEWaitReply );
				break;
			
			default:
				return false;
				break;
		}
		
		return true;
	}
	
	void AdjustSleepForTimer( UInt32 ticksToSleep )
	{
		if ( ticksToSleep < gRunState.maxTicksToSleep )
		{
			gRunState.maxTicksToSleep = ticksToSleep;
		}
	}
	
	void AdjustSleepForActivity()
	{
		gRunState.activelyBusy = true;
	}
	
}

