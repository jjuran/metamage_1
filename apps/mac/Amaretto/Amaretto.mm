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

// mac-app-utils
#include "mac_app/become_application.hh"

// frontend-common
#include "frend/coprocess.hh"
#include "frend/displayfs.hh"
#include "frend/documents.hh"
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


static
void sigchld( int )
{
	frend::unblock_update_waiters();
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
	
	// Reserve fds 6 and 7 for graft to connect ams-fs/xv68k to freemountd.
	dup2( STDERR_FILENO, 6 );
	dup2( STDERR_FILENO, 7 );
	
	const char* works_path = frend::tempfile_location();
	
	int nok = mkdir( works_path, 0777 );
	
	if ( nok  &&  errno != EEXIST )
	{
		return 1;
	}
	
	if ( wait_for_first_Apple_event() == noErr )
	{
		int bindir_fd = bindir( argv[ 0 ] );
		
		frend::scrub_documents_dir( bindir_fd );
		
		chdir( works_path );
		
		cursor_lifetime   live_cursor( cursor_path );
		raster_lifetime   live_raster( raster_path );
		
		const raster_load& load = live_raster.get();
		const raster_desc& desc = live_raster.desc();
		
		releasing appDelegate( [[AmarettoAppDelegate alloc] initWithRaster: load] );
		
		raster_event_target = appDelegate;
		
		raster_updating   update_fifo;
		raster_monitor    monitored_raster( live_raster.get() );
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
