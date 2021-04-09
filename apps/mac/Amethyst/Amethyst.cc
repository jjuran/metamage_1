/*
	Amethyst.cc
	-----------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// POSIX
#include <unistd.h>

// mac-config
#include "mac_config/upp-macros.hh"

// splode
#include "splode/splode.hh"

// mac-app-utils
#include "mac_app/event_handlers.hh"
#include "mac_app/state.hh"

// Amethyst
#include "apple_events.hh"
#include "blit_CG.hh"
#include "blit_QD.hh"
#include "coprocess.hh"
#include "display.hh"
#include "keycodes.hh"
#include "make_raster.hh"
#include "quickdraw.hh"
#include "raster_task.hh"
#include "splode.hh"
#include "tempfile.hh"


#define LENGTH( array )  (sizeof (array) / sizeof *(array))


static int events_fd = -1;

static CGPoint cursor_limit;
static CGPoint last_cursor_location;

static
void move_cursor_to( CGPoint location )
{
	CGWarpMouseCursorPosition( location );
	
	CGAssociateMouseAndMouseCursorPosition( true );
}

static
CGPoint pin_cursor( CGPoint next_cursor_location )
{
	CGPoint unpinned_location = next_cursor_location;
	
	if ( next_cursor_location.x >= cursor_limit.x )
	{
		next_cursor_location.x = cursor_limit.x - 1;
	}
	
	if ( next_cursor_location.y >= cursor_limit.y )
	{
		next_cursor_location.y = cursor_limit.y - 1;
	}
	
	if ( ! CGPointEqualToPoint( next_cursor_location, unpinned_location ) )
	{
		move_cursor_to( next_cursor_location );
	}
	
	return next_cursor_location;
}

static
void handle_mouse_moved_event( CGPoint next_cursor_location )
{
	CGPoint pinned_cursor = pin_cursor( next_cursor_location );
	
	if ( ! CGPointEqualToPoint( pinned_cursor, last_cursor_location ) )
	{
		send_mouse_moved_event( events_fd, pinned_cursor.x, pinned_cursor.y );
		
		last_cursor_location = pinned_cursor;
	}
}

static inline
bool is_mouse_moved_event( const EventRecord& event )
{
	const uint32_t target_message = mouseMovedMessage << 24;
	const uint32_t masked_message = event.message & osEvtMessageMask;
	
	return event.what == osEvt  &&  masked_message == target_message;
}

static
double x_scale_factor( CGRect frame, double width, double height )
{
	double x_factor = frame.size.width  / width;
	double y_factor = frame.size.height / height;
	
	double factor = x_factor < y_factor ? x_factor : y_factor;
	
	if ( factor > 1 )
	{
		factor = (int) factor;
	}
	
	return factor;
}

static
pascal OSStatus eventtap_RawKey( EventHandlerCallRef  handler,
                                 EventRef             event,
                                 void*                userData )
{
	OSStatus err;
	
	UInt32 modifiers = 0;
	err = GetEventParameter( event,
	                         kEventParamKeyModifiers,
	                         typeUInt32,
	                         NULL,
	                         sizeof (UInt32),
	                         NULL,
	                         &modifiers );
	
	using namespace splode::modes;
	using namespace splode::key;
	
	const uint8_t mode_mask = Command | Shift | Option | Control;
	const uint8_t attr_mask = Alpha;
	
	const uint8_t modes = (modifiers >> 8) & mode_mask;
	const uint8_t attrs = (modifiers >> 8) & attr_mask;
	
	send_key_event( events_fd, '\0', modes, attrs );
	
	return noErr;
}

DEFINE_CARBON_UPP( EventHandler, eventtap_RawKey )

static EventTypeSpec rawKey_event[] =
{
	{ kEventClassKeyboard, kEventRawKeyModifiersChanged },
};

#ifdef MAC_OS_X_VERSION_10_5

static
bool handle_CGEvent( CGEventRef event )
{
	CGEventType type = CGEventGetType( event );
	
	const CGEventFlags mask_Option_and_Shift = kCGEventFlagMaskAlternate
	                                         | kCGEventFlagMaskShift;
	
	CGEventFlags flags = CGEventGetFlags( event );
	
	const uint8_t modes = (flags & kCGEventFlagMaskCommand) >> 20
	                    | (flags & mask_Option_and_Shift  ) >> 16
	                    | (flags & kCGEventFlagMaskControl) >> 14;
	
	switch ( type )
	{
		case kCGEventLeftMouseDown:
		case kCGEventLeftMouseUp:
		case kCGEventRightMouseDown:
		case kCGEventRightMouseUp:
		case kCGEventMouseMoved:
		case kCGEventLeftMouseDragged:
		case kCGEventRightMouseDragged:
			handle_mouse_moved_event( CGEventGetLocation( event ) );
			break;
		
		default:
			break;
	}
	
	switch ( type )
	{
		case kCGEventLeftMouseDown:
		case kCGEventLeftMouseUp:
			send_mouse_event( events_fd, type, modes );
			return true;
		
		case kCGEventRightMouseDown:
		case kCGEventRightMouseUp:
			return false;
		
		case kCGEventMouseMoved:
		case kCGEventLeftMouseDragged:
		case kCGEventRightMouseDragged:
			return true;
		
		default:
			break;
	}
	
	uint8_t attrs = (flags & kCGEventFlagMaskAlphaShift) >> 14;
	
	uint8_t c = '\0';
	
	switch ( type )
	{
		case kCGEventKeyDown:
			if ( CGEventGetIntegerValueField( event, kCGKeyboardEventAutorepeat ) )
			{
				attrs += 2;  // augment from key-down to auto-key
			}
			// fall through
		case kCGEventKeyUp:
			attrs += type - (kCGEventKeyDown - 1);  // splode::key::down
			
			int8_t key;
			key = CGEventGetIntegerValueField( event, kCGKeyboardEventKeycode );
			
			c = lookup_from_virtual[ key & 0x7F ];
			
			// fall through
		case kCGEventFlagsChanged:
			send_key_event( events_fd, c, modes, attrs );
			return true;
		
		default:
			break;
	}
	
	return false;
}

#endif  // #ifdef MAC_OS_X_VERSION_10_5

static
bool handle_EventRecord( const EventRecord& event )
{
	Point where = event.where;
	
	switch ( event.what )
	{
		case osEvt:
			if ( ! is_mouse_moved_event( event ) )
			{
				break;
			}
			// fall through
		case mouseDown:
		case mouseUp:
			handle_mouse_moved_event( CGPointMake( where.h, where.v ) );
			break;
	}
	
	switch ( event.what )
	{
		case mouseDown:
		case mouseUp:
			send_mouse_event( events_fd, event );
			break;
		
		case keyDown:
		case keyUp:
		case autoKey:
			send_key_event( events_fd, event );
			break;
		
		case kHighLevelEvent:
			(void) AEProcessAppleEvent( &event );
			break;
		
		default:
			break;
	}
	
	return true;
}

#if CONFIG_QUICKDRAW
	typedef QD_blitter Blitter;
#else
	typedef CG_blitter Blitter;
#endif

static
void RunEventLoop()
{
	OSStatus err;
	
	display_capture captured_display;
	
	CGRect display_bounds = CGDisplayBounds( captured_display.id() );
	
	const char* raster_path = tempfile_location();
	
	raster_lifetime live_raster( raster_path );
	
	void*                      addr = live_raster.addr();
	const raster::raster_desc& desc = live_raster.desc();
	
	double factor = x_scale_factor( display_bounds, desc.width, desc.height );
	
	double x_offset = (display_bounds.size.width  - factor * desc.width ) / 2;
	double y_offset = (display_bounds.size.height - factor * desc.height) / 2;
	
	CGRect bounds = CGRectInset( display_bounds, x_offset, y_offset );
	
	Blitter blitter( captured_display.id(), bounds );
	
	blitter.prep( desc.stride, desc.width, desc.height );
	
	raster_monitor monitored_raster;
	
	coprocess_launch launched_coprocess( raster_path );
	
	events_fd = launched_coprocess.socket();
	
#ifndef MAC_OS_X_VERSION_10_5
	
	/*
		Install a Carbon Event handler to respond to changes to the currently
		active modifier keys.  OS X 10.5+ uses CGEvents and doesn't need this.
	*/
	
	EventHandlerRef rawKeyHandler;
	err = InstallApplicationEventHandler( UPP_ARG( eventtap_RawKey ),
	                                      LENGTH( rawKey_event ),
	                                      rawKey_event,
	                                      NULL,
	                                      &rawKeyHandler );
	
#endif
	
	cursor_limit = CGPointMake( desc.width, desc.height );
	
	CGWarpMouseCursorPosition( CGPointMake( 15, 15 ) );
	
	EventTargetRef target = GetEventDispatcherTarget();
	
	while ( ! mac::app::quitting )
	{
		EventRef event;
		
		const bool pull = true;
		
		err = ReceiveNextEvent( 0, NULL, kEventDurationForever, pull, &event );
		
		if ( err )  return;
		
		OSType eventClass = GetEventClass( event );
		
		if ( eventClass == kEventClassApplication )
		{
			if ( GetEventKind( event ) == kEventAppQuit )
			{
				mac::app::quitting = true;
				
				goto next;
			}
		}
		
		if ( eventClass == kEventClassAmethyst )
		{
			const uint32_t offset = desc.height * desc.stride * desc.frame;
			
			blitter.blit( (Ptr) addr + offset );
			goto next;
		}
		
	#ifdef MAC_OS_X_VERSION_10_5
		
		if ( eventClass == kEventClassAppleEvent )
		{
			err = AEProcessEvent( event );
			
			goto next;
		}
		
		if ( CGEventRef cgevent = CopyEventCGEvent( event ) )
		{
			const bool handled = handle_CGEvent( cgevent );
			
			CFRelease( cgevent );
			
			if ( handled )
			{
				goto next;
			}
		}
		
	#else  // #ifdef MAC_OS_X_VERSION_10_5
		
		EventRecord eventRec;
		
		if ( bool ok = ConvertEventRefToEventRecord( event, &eventRec ) )
		{
			if ( handle_EventRecord( eventRec ) )
			{
				goto next;
			}
		}
		
	#endif  // #ifdef MAC_OS_X_VERSION_10_5
		
		err = SendEventToEventTarget( event, target );
		
	next:
		
		if ( pull )
		{
			ReleaseEvent( event );
		}
	}
	
	/*
		Move the native cursor to the same location where we last saw the
		emulated cursor, so the cursor won't appear to jump when we release
		the captured display.
	*/
	
	CGPoint transformed_location;
	
	transformed_location.x = last_cursor_location.x * factor + x_offset;
	transformed_location.y = last_cursor_location.y * factor + y_offset;
	
	move_cursor_to( transformed_location );
}

static
void change_dir( char* argv0 )
{
	if ( char* slash = strrchr( argv0, '/' ) )
	{
		*slash = '\0';
		
		chdir( argv0 );
		
		*slash = '/';
	}
}

int main( int argc, char** argv )
{
	change_dir( argv[ 0 ] );
	
	mac::app::install_basic_event_handlers();
	
	if ( wait_for_first_Apple_event() == noErr )
	{
		RunEventLoop();
	}
	
	return 0;
}
