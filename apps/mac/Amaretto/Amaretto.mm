/*
	Amaretto.mm
	-----------
*/

// Mac OS X
#import <Cocoa/Cocoa.h>

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <signal.h>

// posix-utils
#include "posix/bindir.hh"

// v68k-cursor
#include "cursor/cursor.hh"

// mac-app-utils
#include "mac_app/become_application.hh"
#include "mac_app/quit.hh"

// glfb-common
#include "glfb/glfb.hh"

// frontend-common
#include "frend/coprocess.hh"
#include "frend/cursor.hh"
#include "frend/displayfs.hh"
#include "frend/make_cursor.hh"
#include "frend/make_raster.hh"
#include "frend/raster_updating.hh"
#include "frend/tempfile.hh"
#include "frend/update_fifo.hh"

// rasterlib
#include "raster/raster.hh"

// amicus
#include "amicus/apple_events.hh"
#include "amicus/events.hh"

// Amaretto
#include "Amaretto/AppDelegate.hh"
#include "raster_task.hh"
#include "releasing.hh"


#define PROGRAM  "Amaretto"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


using mac::app::become_application;

using raster::raster_desc;
using raster::raster_load;

using frend::cursor_path;
using frend::raster_path;

using frend::coprocess_launch;
using frend::cursor_lifetime;
using frend::raster_lifetime;
using frend::raster_updating;

using amicus::cursor_limit;
using amicus::events_fd;
using amicus::wait_for_first_Apple_event;


static void*                      the_addr;
static const raster::raster_desc* the_desc;

static long the_image_size;

static
void sigchld( int )
{
	frend::unblock_update_waiters();
}

static
void on_display_event( void* info )
{
	using frend::cursor_state;
	using frend::shared_cursor_state;
	
	display_event_set& events = *(display_event_set*) info;
	
	if ( events.cursorBits )
	{
		events.cursorBits = false;
		
		if ( cursor_state )
		{
			glfb::set_cursor_image( cursor_state );
		}
	}
	
	if ( events.newPalette )
	{
		events.newPalette = false;
		
		if ( const unsigned short* palette = events.clut_palette )
		{
			glfb::set_palette( palette, events.clut_maximum + 1 );
		}
	}
	
	if ( events.screenBits )
	{
		events.screenBits = false;
		
		const uint32_t offset = the_image_size * the_desc->frame;
		
		glfb::set_screen_image( (Ptr) the_addr + offset );
	}
	
	if ( events.repaintDue )
	{
		events.repaintDue = false;
		
		if ( const shared_cursor_state* cursor = cursor_state )
		{
			int y  = cursor->pointer[ 0 ];
			int x  = cursor->pointer[ 1 ];
			int dy = cursor->hotspot[ 0 ];
			int dx = cursor->hotspot[ 1 ];
			
			glfb::set_cursor_hotspot( dx, dy );
			glfb::set_cursor_location( x, y );
			glfb::set_cursor_visibility( cursor->visible );
		}
		
		glfb::render_and_flush();
	}
	
	if ( events.rasterDone )
	{
		mac::app::quit();
	}
}

int main( int argc, char** argv )
{
	releasing pool( [NSAutoreleasePool new] );
	
	[NSApplication sharedApplication];
	
	if ( ! become_application() )
	{
		WARN( "Can't become an application, exiting." );
		
		_exit( 1 );
	}
	
	signal( SIGCHLD, &sigchld );
	
	const char* works_path = frend::tempfile_location();
	
	int nok = mkdir( works_path, 0777 );
	
	if ( nok  &&  errno != EEXIST )
	{
		return 1;
	}
	
	if ( wait_for_first_Apple_event() == noErr )
	{
		int bindir_fd = bindir( argv[ 0 ] );
		
		chdir( works_path );
		
		cursor_lifetime   live_cursor( cursor_path );
		raster_lifetime   live_raster( raster_path );
		
		const raster_load& load = live_raster.get();
		const raster_desc& desc = live_raster.desc();
		
		the_addr = load.addr;
		the_desc = &desc;
		
		the_image_size = desc.stride * desc.height;
		
		glfb::cursor_enabled = frend::cursor_state != NULL;
		
		releasing appDelegate( [[AmarettoAppDelegate alloc] initWithRaster: load] );
		
		display_monitor::perform_proc perform = &on_display_event;
		
		raster_updating   update_fifo;
		display_monitor   monitored_display( live_raster.get(), perform );
		coprocess_launch  launched_coprocess( bindir_fd, works_path );
		
		events_fd = launched_coprocess.socket();
		
		close( bindir_fd );
		
		cursor_limit = CGPointMake( desc.width - 1, desc.height - 1 );
		
		[NSApp setDelegate: appDelegate];
		
		[NSApp run];
		
		close( events_fd );
	}
	
	rmdir( works_path );
	
	return 0;
}
