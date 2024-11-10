/*
	shared.cc.hh
	------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// frontend-common
#include "frend/coprocess.hh"
#include "frend/make_cursor.hh"
#include "frend/make_raster.hh"
#include "frend/raster_updating.hh"
#include "frend/update_fifo.hh"

// amicus
#include "amicus/apple_events.hh"
#include "amicus/events.hh"
#include "amicus/raster_task.hh"
#include "amicus/tempfile.hh"


#ifndef HARDWARE_CURSOR
#define HARDWARE_CURSOR 0
#endif

static
void sigchld( int )
{
	open( UPDATE_FIFO, O_RDONLY | O_NONBLOCK );
}

namespace amicus
{

using raster::raster_desc;
using raster::raster_load;

static const char raster_path[] = "screen.skif";
static const char cursor_path[] = "cursor.skif";

class emulated_screen
{
	typedef frend::coprocess_launch coprocess_launch;
	typedef frend::cursor_lifetime  cursor_lifetime;
	typedef frend::raster_lifetime  raster_lifetime;
	typedef frend::raster_updating  raster_updating;
	
	private:
	#if HARDWARE_CURSOR
		cursor_lifetime   live_cursor;
	#endif
		raster_lifetime   live_raster;
		raster_updating   update_fifo;
		raster_monitor    monitored_raster;
		coprocess_launch  launched_coprocess;
	
	public:
		emulated_screen( int bindir_fd, const char* works_path );
		
		const raster_load& load() const  { return live_raster.get(); }
		const raster_desc& desc() const  { return live_raster.desc(); }
};

emulated_screen::emulated_screen( int bindir_fd, const char* works_path )
:
#if HARDWARE_CURSOR
	live_cursor       ( cursor_path ),
#endif
	live_raster       ( raster_path ),
	monitored_raster  ( live_raster.get() ),
	launched_coprocess( bindir_fd, works_path )
{
	events_fd = launched_coprocess.socket();
}

void initialize();

void run_event_loop( const raster_load& load, const raster_desc& desc );

}  // namespace amicus

static
int bindir( char* argv0 )
{
	int dirfd;
	
	if ( char* slash = strrchr( argv0, '/' ) )
	{
		*slash = '\0';
		
		dirfd = open( argv0, O_RDONLY | O_DIRECTORY );
		
		*slash = '/';
	}
	else
	{
		dirfd = open( ".", O_RDONLY | O_DIRECTORY );
	}
	
	return dirfd;
}

int main( int argc, char** argv )
{
	signal( SIGCHLD, &sigchld );
	
	// Reserve fds 6 and 7 for graft to connect ams-fs/xv68k to freemountd.
	dup2( STDERR_FILENO, 6 );
	dup2( STDERR_FILENO, 7 );
	
	amicus::initialize();
	
	using namespace amicus;
	
	const char* works_path = tempfile_location();
	
	int nok = mkdir( works_path, 0777 );
	
	if ( nok  &&  errno != EEXIST )
	{
		return 1;
	}
	
	if ( wait_for_first_Apple_event() == noErr )
	{
		int bindir_fd = bindir( argv[ 0 ] );
		
		chdir( works_path );
		
		emulated_screen screen( bindir_fd, works_path );
		
		close( bindir_fd );
		
		const raster_load& load = screen.load();
		const raster_desc& desc = screen.desc();
		
		run_event_loop( load, desc );
	}
	
	rmdir( works_path );
	
	return 0;
}
