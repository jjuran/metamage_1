/*
	make_raster.hh
	--------------
*/

#ifndef FREND_MAKERASTER_HH
#define FREND_MAKERASTER_HH

// iota
#include "iota/class.hh"

// rasterlib
#include "raster/raster.hh"


namespace frend
{

struct raster_creation_error {};

class raster_lifetime
{
	NON_COPYABLE( raster_lifetime )
	
	typedef raster::raster_load raster_load;
	
	private:
		const char* its_path;
		raster_load its_load;
	
	public:
		raster_lifetime( const char* raster_path );
		
		~raster_lifetime();
		
		const raster::raster_load& get() const  { return its_load; }
		
		void* addr() const  { return get().addr; }
		
		const raster::raster_desc& desc() const { return get().meta->desc; }
};

}

#endif
