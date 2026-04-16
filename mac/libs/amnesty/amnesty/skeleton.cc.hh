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

// rasterlib
#include "raster/clut.hh"
#include "raster/clut_detail.hh"

// frontend-common
#include "frend/commandmode_state.hh"
#include "frend/display_events.hh"
#include "frend/zoom.hh"

// amicus
#include "amicus/events.hh"
#include "amicus/fade.hh"

#define CAPTURING_DISPLAY  1

#include "amicus/shared.cc.hh"

// amnesty
#include "amnesty/display.hh"


#define LENGTH( array )  (sizeof (array) / sizeof *(array))

#ifdef MAC_OS_X_VERSION_10_5
	#define CONFIG_CGEVENTS  1
	
	typedef CGEventRef LowLevelEvent;
	
	#define CONVERT_EVENT( event, ll )  (ll = CopyEventCGEvent( event ))
	
	#define HANDLE_EVENT  handle_CGEvent
	
	#define RELEASE_EVENT( ll )  CFRelease( ll )
#else
	#define CONFIG_CGEVENTS  0
	
	typedef EventRecord LowLevelEvent;
	
	#define CONVERT_EVENT( e, ll )  ConvertEventRefToEventRecord( e, &ll )
	
	#define HANDLE_EVENT  handle_EventRecord
	
	#define RELEASE_EVENT( ll )  /**/
#endif


namespace amicus
{

using frend::clear_chords;
using frend::CommandMode_state;
using frend::commandmode_state;
using frend::display_events;
using frend::quick_exit;

using frend::current_zoom_index;
using frend::maximum_zoom_index;
using frend::minimum_zoom_index;

static double max_scale_factor = 1;
static double scale_factor = 1;

static
void command_handler( char c )
{
	if ( frend::commandmode_key( c ) )
	{
		if ( quick_exit() )
		{
			mac::app::quitting = true;
		}
	}
}

static
double max_scale( CGRect frame, double width, double height )
{
	double x_factor = frame.size.width  / width;
	double y_factor = frame.size.height / height;
	
	double factor = x_factor < y_factor ? x_factor : y_factor;
	
	return floor( factor * 2 ) / 2;
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
	
	Blitter::render();
}

static
pascal OSStatus Modifiers_changed( EventHandlerCallRef  handler,
                                   EventRef             event,
                                   void*                userData )
{
	OSStatus err;
	
	CommandMode_state prev_state = commandmode_state;
	
	err = amicus::send_key_event( event, '\0' );
	
	if ( ! commandmode_state )
	{
		clear_chords();
	}
	
	if ( ! commandmode_state != ! prev_state )
	{
		overlay_enabled = ! overlay_enabled;
		
		Blitter::render();
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
		an assertion failure in Ventura's Skylight framework during
		the call to SendEventToEventTarget().
		
		HIToolbox's HandleAppShowHide() calls _ReorderWindowsTogether(),
		which calls Skylight's SLSReorderWindows().  Since applications
		using amnesty don't have any windows at all, their order seems
		unimportant, and these events can probably be safely discarded.
		
		https://github.com/jjuran/metamage_1/issues/26
	*/
	
	return eventClass == kEventClassApplication  &&  eventKind == 103;
}

static
void set_palette( Blitter& blitter, const raster_load& load )
{
	if ( const raster::clut_data* clut = find_clut( &load.meta->note ) )
	{
		blitter.set_palette( &clut->palette[ 0 ].value, clut->max + 1 );
	}
}

static
void quit()
{
	OSStatus err;
	EventRef quitEvent;
	
	EventQueueRef queue = GetMainEventQueue();
	
	err = CreateEvent( NULL,
	                   kEventClassApplication,
	                   kEventAppQuit,
	                   0,
	                   kEventAttributeNone,
	                   &quitEvent );
	
	err = PostEventToQueue( queue, quitEvent, kEventPriorityHigh );
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
	
	max_scale_factor = max_scale( display_bounds, width, height );
	
	scale_factor = max_scale_factor >= 2.0 ? 2.0
	             : max_scale_factor >= 1.5 ? 1.5
	             :                           1.0;
	
	minimum_zoom_index = 0x100 >> 7;  // 100%
	maximum_zoom_index = (int) floor( max_scale_factor * 2 );
	current_zoom_index = (int) floor(     scale_factor * 2 );
	
	Blitter blitter( captured_display.id() );
	
	set_palette( blitter, load );
	
	blitter.prep( stride, width, height, desc.weight );
	
	blitter.area( display_area( display_bounds, width, height ) );
	
	/*
		Install a Carbon Event handler to respond to changes to the currently
		active modifier keys.  OS X 10.5+ uses CGEvents and doesn't need this.
	*/
	
	if ( ! CONFIG_CGEVENTS )
	{
		EventHandlerRef modifiersHandler;
		
		err = InstallApplicationEventHandler( UPP_ARG( Modifiers_changed ),
		                                      LENGTH( Modifiers_event ),
		                                      Modifiers_event,
		                                      NULL,
		                                      &modifiersHandler );
	}
	
	cursor_limit = CGPointMake( width - 1, height - 1 );
	
	last_cursor_location = CGPointMake( 15, 15 );
	
#ifndef MAC_OS_X_VERSION_10_5
	
	CGWarpMouseCursorPosition( last_cursor_location );
	
#endif
	
	display_events.finish = &quit;
	display_events.render = &Blitter::render;
	
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
		
	#ifdef MAC_OS_X_VERSION_10_5
		
		if ( eventClass == kEventClassAppleEvent )
		{
			err = AEProcessEvent( event );
			
			goto next;
		}
		
	#endif
		
		LowLevelEvent lowlevel_event;
		
		if ( CONVERT_EVENT( event, lowlevel_event ) )
		{
			CommandMode_state prev_state = commandmode_state;
			
			const int previous_zoom_index = current_zoom_index;
			
			bool handled = HANDLE_EVENT( lowlevel_event, &command_handler );
			
			RELEASE_EVENT( lowlevel_event );
			
			if ( CONFIG_CGEVENTS  &&  ! commandmode_state )
			{
				clear_chords();
			}
			
			if ( ! commandmode_state != ! prev_state )
			{
				overlay_enabled = ! overlay_enabled;
				
				Blitter::render();
			}
			else if ( current_zoom_index != previous_zoom_index )
			{
				scale_factor = current_zoom_index / 2.0;
				
				blitter.area( display_area( display_bounds, width, height ) );
				
				blit( load );
			}
			
			if ( handled )
			{
				goto next;
			}
		}
		
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
