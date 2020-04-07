/*
	raster_lock.hh
	--------------
*/

#ifndef RASTERLOCK_HH
#define RASTERLOCK_HH

class raster_lock
{
	private:
		const bool cursor_locked;
		const bool screen_locked;
		
		// non-copyable
		raster_lock           ( const raster_lock& );
		raster_lock& operator=( const raster_lock& );
	
	public:
		raster_lock( bool cursor = true, bool screen = true );
		
		~raster_lock();
};

#endif
