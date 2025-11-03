/*
	make_cursor.hh
	--------------
*/

#ifndef FREND_MAKECURSOR_HH
#define FREND_MAKECURSOR_HH

// iota
#include "iota/class.hh"

// rasterlib
#include "raster/raster.hh"


namespace frend
{

struct cursor_creation_error {};

class cursor_lifetime
{
	NON_COPYABLE( cursor_lifetime )
	
	typedef raster::raster_load raster_load;
	
	private:
		const char* its_path;
		raster_load its_load;
	
	public:
		cursor_lifetime( const char* cursor_path );
		
		~cursor_lifetime();
		
		const raster::raster_load& get() const  { return its_load; }
		
		void* addr() const  { return get().addr; }
		
		const raster::raster_desc& desc() const { return get().meta->desc; }
};

}

#endif
