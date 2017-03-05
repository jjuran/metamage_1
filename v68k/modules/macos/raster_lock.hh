/*
	raster_lock.hh
	--------------
*/

#ifndef RASTERLOCK_HH
#define RASTERLOCK_HH


void lock_raster();
void unlock_raster();

class raster_lock
{
	private:
		// non-copyable
		raster_lock           ( const raster_lock& );
		raster_lock& operator=( const raster_lock& );
	
	public:
		raster_lock()
		{
			lock_raster();
		}
		
		~raster_lock()
		{
			unlock_raster();
		}
};

#endif
