/*
	shared.cc.hh
	------------
*/

// POSIX
#include <unistd.h>

// amicus
#include "amicus/apple_events.hh"
#include "amicus/coprocess.hh"
#include "amicus/make_raster.hh"
#include "amicus/raster_task.hh"
#include "amicus/tempfile.hh"


namespace amicus
{

class emulated_screen
{
	private:
		raster_lifetime   live_raster;
		raster_monitor    monitored_raster;
		coprocess_launch  launched_coprocess;
	
	public:
		explicit emulated_screen( const char* raster_path );
		
		const raster::raster_load& load() const  { return live_raster.get(); }
		const raster::raster_desc& desc() const  { return live_raster.desc(); }
};

emulated_screen::emulated_screen( const char* raster_path )
:
	live_raster       ( raster_path ),
	launched_coprocess( raster_path )
{
	events_fd = launched_coprocess.socket();
}

static
void run_event_loop( const emulated_screen& screen );

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

static
void initialize();

int main( int argc, char** argv )
{
	change_dir( argv[ 0 ] );
	
	initialize();
	
	using namespace amicus;
	
	if ( wait_for_first_Apple_event() == noErr )
	{
		emulated_screen screen( tempfile_location() );
		
		run_event_loop( screen );
	}
	
	return 0;
}
