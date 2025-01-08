/*
	screen.hh
	---------
*/

#ifndef PEARL_SCREEN_HH
#define PEARL_SCREEN_HH

// POSIX
#include <sys/stat.h>

// frontend-common
#include "frend/coprocess.hh"
#include "frend/make_cursor.hh"
#include "frend/make_raster.hh"
#include "frend/raster_updating.hh"


namespace Pearl
{

using raster::raster_desc;
using raster::raster_load;

class raster_monitor
{
	private:
		// non-copyable
		raster_monitor           ( const raster_monitor& );
		raster_monitor& operator=( const raster_monitor& );

	public:
		explicit raster_monitor( const raster::raster_load& load );

		~raster_monitor();
};

class emulated_screen
{
	typedef frend::coprocess_launch coprocess_launch;
	typedef frend::cursor_lifetime  cursor_lifetime;
	typedef frend::raster_lifetime  raster_lifetime;
	typedef frend::raster_updating  raster_updating;

	private:
		cursor_lifetime   live_cursor;
		raster_lifetime   live_raster;
		raster_updating   update_fifo;
		raster_monitor    monitored_raster;
		coprocess_launch  launched_coprocess;

	public:
		emulated_screen( int bindir_fd, const char* works_path );

		const raster_load& load() const  { return live_raster.get(); }
		const raster_desc& desc() const  { return live_raster.desc(); }
};

}

#endif
