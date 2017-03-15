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
		const bool condition;
		
		// non-copyable
		raster_lock           ( const raster_lock& );
		raster_lock& operator=( const raster_lock& );
	
	public:
		raster_lock( bool cond = true ) : condition( cond )
		{
			if ( cond )
			{
				lock_raster();
			}
		}
		
		~raster_lock()
		{
			if ( condition )
			{
				unlock_raster();
			}
		}
};

#endif
