/*
	raster.hh
	---------
*/

#ifndef RASTER_HH
#define RASTER_HH

// Standard C
#include <stdint.h>

// rasterlib
#include "raster/raster.hh"
#include "raster/relay.hh"

// telekine
#include "telekine/types.hh"


namespace telekine
{
	
	class raster_task
	{
		private:
			raster::raster_load  its_raster;
			raster::sync_relay*  its_sync;
			
			// non-copyable
			raster_task           ( const raster_task& );
			raster_task& operator=( const raster_task& );
		
		public:
			raster_task( const char* path, const raster_desc& desc );
			~raster_task();
			
			const raster::raster_desc& desc() const
			{
				return its_raster.meta->desc;
			}
			
			void recv_rect( int fd, const rect_pv16& r ) const;
	};
	
}

#endif
