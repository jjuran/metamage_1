/*
	make_cursor.hh
	--------------
*/

#ifndef AMICUS_MAKECURSOR_HH
#define AMICUS_MAKECURSOR_HH

// raster
#include "raster/raster.hh"


namespace amicus
{

struct cursor_creation_error {};

class cursor_lifetime
{
	typedef raster::raster_load raster_load;
	
	private:
		const char* its_path;
		raster_load its_load;
		
		// non-copyable
		cursor_lifetime           ( const cursor_lifetime& );
		cursor_lifetime& operator=( const cursor_lifetime& );
	
	public:
		cursor_lifetime( const char* cursor_path );
		
		~cursor_lifetime();
		
		const raster::raster_load& get() const  { return its_load; }
		
		void* addr() const  { return get().addr; }
		
		const raster::raster_desc& desc() const { return get().meta->desc; }
};

}

#endif
