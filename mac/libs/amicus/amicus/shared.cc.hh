/*
	shared.cc.hh
	------------
*/

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// amicus
#include "amicus/apple_events.hh"
#include "amicus/coprocess.hh"
#include "amicus/make_raster.hh"
#include "amicus/raster_task.hh"
#include "amicus/tempfile.hh"


namespace amicus
{

static const char raster_path[] = "screen.skif";

class emulated_screen
{
	private:
		raster_lifetime   live_raster;
		raster_monitor    monitored_raster;
		coprocess_launch  launched_coprocess;
	
	public:
		emulated_screen( int bindir_fd, const char* works_path );
		
		const raster::raster_load& load() const  { return live_raster.get(); }
		const raster::raster_desc& desc() const  { return live_raster.desc(); }
};

emulated_screen::emulated_screen( int bindir_fd, const char* works_path )
:
	live_raster       ( raster_path ),
	monitored_raster  ( live_raster.get() ),
	launched_coprocess( bindir_fd, works_path )
{
	events_fd = launched_coprocess.socket();
}

static
void run_event_loop( const emulated_screen& screen );

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

static
void initialize();

int main( int argc, char** argv )
{
	// Reserve fds 6 and 7 for graft to connect ams-fs/xv68k to freemountd.
	dup2( STDERR_FILENO, 6 );
	dup2( STDERR_FILENO, 7 );
	
	initialize();
	
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
		
		run_event_loop( screen );
	}
	
	rmdir( works_path );
	
	return 0;
}
