/*
	raster_task.hh
	--------------
*/

#ifndef AMICUS_RASTERTASK_HH
#define AMICUS_RASTERTASK_HH


#define UPDATE_FIFO  "update-signal.fifo"


namespace raster
{
	
	struct raster_load;
	
}

namespace amicus
{

enum
{
	kEventClassAmicus = 'AMS*',
	
	kEventAmicusUpdate = 1,
	kEventAmicusScreenBits = 2,
	kEventAmicusCursorBits = 3,
};

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

}

#endif
