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
	
	double factor = x_scale_factor( display_bounds, width, height );
	
	double x_offset = (display_bounds.size.width  - factor * width ) / 2;
	double y_offset = (display_bounds.size.height - factor * height) / 2;
	
	CGRect bounds = CGRectInset( display_bounds, x_offset, y_offset );
	
	Blitter blitter( captured_display.id(), bounds );
	
	blitter.prep( stride, width, height );
	
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
			
			const bool handled = handle_CGEvent( cgevent );
			
			CFRelease( cgevent );
			
			if ( ! commandmode_state != ! prev_state )
			{
				overlay_enabled = ! overlay_enabled;
				
				const uint32_t offset = height * stride * desc.frame;
				
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
			
			const bool handled = handle_EventRecord( eventRec );
			
			if ( ! commandmode_state != ! prev_state )
			{
				overlay_enabled = ! overlay_enabled;
				
				const uint32_t offset = height * stride * desc.frame;
				
				blitter.blit( (Ptr) addr + offset );
			}
			
			if ( handled )
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
	
	fader.begin();
	
	/*
		Move the native cursor to the same location where we last saw the
		emulated cursor, so the cursor won't appear to jump when we release
		the captured display.
	*/
	
	CGPoint transformed_location;
	
	transformed_location.x = last_cursor_location.x * factor + x_offset;
	transformed_location.y = last_cursor_location.y * factor + y_offset;
	
	move_cursor_to( transformed_location );
	
	fader.end();
}

void initialize()
{
	mac::app::install_basic_event_handlers();
}

}  // namespace amicus
