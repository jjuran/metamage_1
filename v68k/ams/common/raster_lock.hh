/*
	raster_lock.hh
	--------------
*/

#ifndef RASTERLOCK_HH
#define RASTERLOCK_HH

class raster_lock
{
	private:
		const bool condition;
		
		// non-copyable
		raster_lock           ( const raster_lock& );
		raster_lock& operator=( const raster_lock& );
	
	public:
		raster_lock( bool cond = true );
		
		~raster_lock();
};

#endif
