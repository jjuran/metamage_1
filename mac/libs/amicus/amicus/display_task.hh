/*
	display_task.hh
	---------------
*/

#ifndef AMICUS_DISPLAYTASK_HH
#define AMICUS_DISPLAYTASK_HH

// iota
#include "iota/class.hh"


namespace raster
{
	
	struct raster_load;
	
}

namespace amicus
{

class display_monitor
{
	NON_COPYABLE( display_monitor )
	
	public:
		typedef void (*perform_proc)( void* info );
		
		display_monitor( const raster::raster_load&  load,
		                 perform_proc                perform );
		
		~display_monitor();
};

}

#endif
