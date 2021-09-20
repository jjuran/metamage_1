/*
	make_raster.hh
	--------------
*/

#ifndef AMICUS_MAKERASTER_HH
#define AMICUS_MAKERASTER_HH

// raster
#include "raster/raster.hh"


namespace amicus
{

extern raster::raster_load loaded_raster;

struct raster_creation_error {};

class raster_lifetime
{
	private:
		const char* its_path;
		
		// non-copyable
		raster_lifetime           ( const raster_lifetime& );
		raster_lifetime& operator=( const raster_lifetime& );
	
	public:
		raster_lifetime( const char* raster_path );
		
		~raster_lifetime();
		
		const raster::raster_load& get() const  { return loaded_raster; }
		
		void* addr() const  { return get().addr; }
		
		const raster::raster_desc& desc() const { return get().meta->desc; }
};

}

#endif
