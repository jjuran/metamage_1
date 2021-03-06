/*
	skeleton.cc.hh
	--------------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// POSIX
#include <unistd.h>

// mac-app-utils
#include "mac_app/event_handlers.hh"
#include "mac_app/state.hh"

// amicus
#include "amicus/apple_events.hh"
#include "amicus/coprocess.hh"
#include "amicus/display.hh"
#include "amicus/events.hh"
#include "amicus/make_raster.hh"
#include "amicus/raster_task.hh"
#include "amicus/tempfile.hh"


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
void run_event_loop()
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
	
	amicus::install_rawKey_handler();
	
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
		
		if ( eventClass == kEventClassAmicus )
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

}  // namespace amicus

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
	
	if ( amicus::wait_for_first_Apple_event() == noErr )
	{
		amicus::run_event_loop();
	}
	
	return 0;
}
