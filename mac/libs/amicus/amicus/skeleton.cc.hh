/*
	skeleton.cc.hh
	--------------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-app-utils
#include "mac_app/event_handlers.hh"
#include "mac_app/state.hh"

// amicus
#include "amicus/display.hh"
#include "amicus/events.hh"
#include "amicus/fade.hh"

#include "amicus/shared.cc.hh"


#define LENGTH( array )  (sizeof (array) / sizeof *(array))


namespace amicus
{

static double max_scale_factor = 1;
static double scale_factor = 1;

static bool Q_hit;
static bool X_hit;

static
void clear_chords()
{
	Q_hit = false;
	X_hit = false;
}

static
void command_handler( char c )
{
	switch ( c )
	{
		case 'q':  Q_hit = true;  break;
		case 'x':  X_hit = true;  break;
		
		case '-':
			if ( scale_factor * 2 > floor( max_scale_factor * 2 ) )
			{
				scale_factor = floor( max_scale_factor * 2 ) / 2;
				break;
			}
			
			scale_factor = floor( scale_factor * 2 - 1 ) / 2;
			
			if ( scale_factor < 1 )
			{
				scale_factor = 1;
			}
			break;
		
		case '=':  // +
			scale_factor = floor( scale_factor * 2 + 1 ) / 2;
			
			if ( scale_factor > max_scale_factor )
			{
				scale_factor = max_scale_factor;
			}
			break;
		
		default:
			break;
	}
	
	if ( Q_hit  &&  X_hit )
	{
		mac::app::quitting = true;
	}
}

static
double max_scale( CGRect frame, double width, double height )
{
	double x_factor = frame.size.width  / width;
	double y_factor = frame.size.height / height;
	
	double factor = x_factor < y_factor ? x_factor : y_factor;
	
	return factor;
}

static
CGRect display_area( CGRect display_bounds, int width, int height )
{
	double x_offset = (display_bounds.size.width  - scale_factor * width ) / 2;
	double y_offset = (display_bounds.size.height - scale_factor * height) / 2;
	
	return CGRectInset( display_bounds, x_offset, y_offset );
}

static
void blit( const raster_load& load )
{
	const raster_desc& desc = load.meta->desc;
	
	const uint32_t offset = desc.height * desc.stride * desc.frame;
	
	Blitter::blit( (Ptr) load.addr + offset );
}

static
pascal OSStatus Modifiers_changed( EventHandlerCallRef  handler,
                                   EventRef             event,
                                   void*                userData )
{
	OSStatus err;
	
	const raster_load& load = *(raster_load*) userData;
	
	CommandMode_state prev_state = commandmode_state;
	
	err = amicus::send_key_event( event, '\0' );
	
	if ( ! commandmode_state )
	{
		clear_chords();
	}
	
	if ( ! commandmode_state != ! prev_state )
	{
		overlay_enabled = ! overlay_enabled;
		
		blit( load );
	}
	
	return err;
}

DEFINE_CARBON_UPP( EventHandler, Modifiers_changed )

static EventTypeSpec Modifiers_event[] =
{
	{ kEventClassKeyboard, kEventRawKeyModifiersChanged },
};

static
bool event_crashes_Ventura( EventRef event )
{
	OSType eventClass = GetEventClass( event );
	UInt32 eventKind  = GetEventKind ( event );
	
	/*
		An event sent when an application is launched normally (as
		opposed to running its executable from a terminal) triggers
		an assertion failure in Ventura's Skylight framework  during
		the call to SendEventToEventTarget().
		
		HIToolbox's HandleAppShowHide() calls _ReorderWindowsTogether(),
		which calls Skylight's SLSReorderWindows().  Since applications
		using amicus don't have any windows at all, their order seems
		unimportant, and these events can probably be safely discarded.
		
		https://github.com/jjuran/metamage_1/issues/26
	*/
	
	return eventClass == kEventClassApplication  &&  eventKind == 103;
}

void run_event_loop( const raster_load& load, const raster_desc& desc )
{
	OSStatus err;
	
	transition_fader fader;
	
	fader.begin();
	
	display_capture captured_display;
	
	fader.end();
	
	CGRect display_bounds = CGDisplayBounds( captured_display.id() );
	
	const int width  = desc.width;
	const int height = desc.height;
	const int stride = desc.stride;
	
	void* addr = load.addr;
	
	max_scale_factor = max_scale( display_bounds, width, height );
	
	if ( max_scale_factor > 1 )
	{
		scale_factor = floor( max_scale_factor );
	}
	
	Blitter blitter( captured_display.id() );
	
	blitter.prep( stride, width, height );
	
	blitter.area( display_area( display_bounds, width, height ) );
	
#ifndef MAC_OS_X_VERSION_10_5
	
	/*
		Install a Carbon Event handler to respond to changes to the currently
		active modifier keys.  OS X 10.5+ uses CGEvents and doesn't need this.
	*/
	
	EventHandlerRef modifiersHandler;
	err = InstallApplicationEventHandler( UPP_ARG( Modifiers_changed ),
	                                      LENGTH( Modifiers_event ),
	                                      Modifiers_event,
	                                      (void*) &load,
	                                      &modifiersHandler );
	
#endif
	
	cursor_limit = CGPointMake( width, height );
	
	last_cursor_location = CGPointMake( 15, 15 );
	
#ifndef MAC_OS_X_VERSION_10_5
	
	CGWarpMouseCursorPosition( last_cursor_location );
	
#endif
	
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
		
		if ( eventClass == kEventClassAmicus )
		{
			const uint32_t offset = height * stride * desc.frame;
			
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
			CommandMode_state prev_state = commandmode_state;
			
			const double prev_scale = scale_factor;
			
			const bool handled = handle_CGEvent( cgevent, &command_handler );
			
			CFRelease( cgevent );
			
			if ( ! commandmode_state )
			{
				clear_chords();
			}
			
			if ( ! commandmode_state != ! prev_state )
			{
				overlay_enabled = ! overlay_enabled;
				
				const uint32_t offset = height * stride * desc.frame;
				
				blitter.blit( (Ptr) addr + offset );
			}
			else if ( scale_factor != prev_scale )
			{
				const uint32_t offset = height * stride * desc.frame;
				
				blitter.area( display_area( display_bounds, width, height ) );
				
				blitter.blit( (Ptr) addr + offset );
			}
			
			if ( handled )
			{
				goto next;
			}
		}
		
	#else  // #ifdef MAC_OS_X_VERSION_10_5
		
		EventRecord eventRec;
		
		if ( bool ok = ConvertEventRefToEventRecord( event, &eventRec ) )
		{
			CommandMode_state prev_state = commandmode_state;
			
			const double prev_scale = scale_factor;
			
			bool handled = handle_EventRecord( eventRec, &command_handler );
			
			if ( ! commandmode_state != ! prev_state )
			{
				overlay_enabled = ! overlay_enabled;
				
				const uint32_t offset = height * stride * desc.frame;
				
				blitter.blit( (Ptr) addr + offset );
			}
			else if ( scale_factor != prev_scale )
			{
				const uint32_t offset = height * stride * desc.frame;
				
				blitter.area( display_area( display_bounds, width, height ) );
				
				blitter.blit( (Ptr) addr + offset );
			}
			
			if ( handled )
			{
				goto next;
			}
		}
		
	#endif  // #ifdef MAC_OS_X_VERSION_10_5
		
		if ( ! event_crashes_Ventura( event ) )
		{
			err = SendEventToEventTarget( event, target );
		}
		
	next:
		
		if ( pull )
		{
			ReleaseEvent( event );
		}
	}
	
	close( events_fd );
	
	fader.begin();
	
	/*
		Move the native cursor to the same location where we last saw the
		emulated cursor, so the cursor won't appear to jump when we release
		the captured display.
	*/
	
	CGPoint transformed_location;
	
	double x_offset = (display_bounds.size.width  - scale_factor * width ) / 2;
	double y_offset = (display_bounds.size.height - scale_factor * height) / 2;
	
	transformed_location.x = last_cursor_location.x * scale_factor + x_offset;
	transformed_location.y = last_cursor_location.y * scale_factor + y_offset;
	
	move_cursor_to( transformed_location );
	
	fader.end();
}

void initialize()
{
	mac::app::install_basic_event_handlers();
}

}  // namespace amicus
