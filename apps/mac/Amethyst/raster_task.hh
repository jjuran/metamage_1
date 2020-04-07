/*
	raster_task.hh
	--------------
*/

#ifndef RASTERTASK_HH
#define RASTERTASK_HH

enum
{
	kEventClassAmethyst = 'AMS*',
	
	kEventAmethystUpdate = 1,
};

class raster_monitor
{
	private:
		// non-copyable
		raster_monitor           ( const raster_monitor& );
		raster_monitor& operator=( const raster_monitor& );
	
	public:
		raster_monitor();
		
		~raster_monitor();
};

#endif
