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
#include "frend/cursor.hh"
#include "frend/display_events.hh"
#include "frend/displayfs.hh"
#include "frend/make_cursor.hh"
#include "frend/make_raster.hh"
#include "frend/raster_updating.hh"
#include "frend/tempfile.hh"
#include "frend/update_fifo.hh"

// glitter
#include "glitter/display_events.hh"

// rasterlib
#include "raster/clut.hh"
#include "raster/clut_detail.hh"
#include "raster/raster.hh"

// amicus
#include "amicus/apple_events.hh"
#include "amicus/display_task.hh"

// Amaretto
#include "Amaretto/AppDelegate.hh"
#include "Amaretto/Application.h"
#include "releasing.hh"


#define PROGRAM  "Amaretto"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


using mac::app::become_application;

using raster::raster_desc;
using raster::raster_load;

using frend::cursor_path;
using frend::raster_path;

using frend::display_events;

using frend::cursor_lifetime;
using frend::raster_lifetime;
using frend::raster_updating;

using glitter::on_display_event;

using amicus::display_monitor;
using amicus::wait_for_first_Apple_event;


static
void sigchld( int )
{
	frend::unblock_update_waiters();
}

static
void set_palette( const raster::raster_load& load )
{
	using namespace raster;
	
	if ( const clut_data* clut = find_clut( &load.meta->note ) )
	{
		glfb::set_palette( &clut->palette[ 0 ].value, clut->max + 1 );
	}
}

int main( int argc, char** argv )
{
	releasing pool( [NSAutoreleasePool new] );
	
	[AmarettoApplication sharedApplication];
	
	if ( ! become_application() )
	{
		WARN( "Can't become an application, exiting." );
		
		_exit( 1 );
	}
	
	signal( SIGCHLD, &sigchld );
	
	works_path = frend::tempfile_location();
	
	int nok = mkdir( works_path, 0777 );
	
	if ( nok  &&  errno != EEXIST )
	{
		return 1;
	}
	
	if ( wait_for_first_Apple_event() == noErr )
	{
		bindir_fd = bindir( argv[ 0 ] );
		
		chdir( works_path );
		
		cursor_lifetime   live_cursor( cursor_path );
		raster_lifetime   live_raster( raster_path );
		
		const raster_load& load = live_raster.get();
		const raster_desc& desc = live_raster.desc();
		
		display_events.finish = &mac::app::quit_Cocoa;
		display_events.render = &glfb::render_and_flush;
		
		glfb::cursor_enabled = frend::cursor_state != NULL;
		
		set_palette( load );
		
		releasing appDelegate( [[AmarettoAppDelegate alloc] initWithRaster: load] );
		
		display_monitor::perform_proc perform = &on_display_event;
		
		raster_updating   update_fifo;
		display_monitor   monitored_display( live_raster.get(), perform );
		
		[NSApp setDelegate: appDelegate];
		
		[NSApp run];
	}
	
	rmdir( works_path );
	
	return 0;
}
