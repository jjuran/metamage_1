/*
	raster.hh
	---------
*/

#ifndef RASTER_HH
#define RASTER_HH

// poseven
#include "poseven/types/cond.hh"
#include "poseven/types/thread.hh"

// rasterlib
#include "raster/raster.hh"
#include "raster/relay.hh"

// telekine
#include "telekine/types.hh"


namespace telekine
{
	
	rect_pv16 buffer_diff_rect( char* one, char* two, raster::raster_desc& desc );
	
	class raster_task
	{
		private:
			raster::raster_load  its_raster;
			raster::sync_relay*  its_sync;
			char*                its_buffer;
			poseven::mutex&      its_ready_mutex;
			poseven::cond&       its_ready_cond;
			poseven::thread      its_thread;
			const char*          its_feed_path;
			int                  its_feed_fd;
			
			void ready();
			
			static void* wait_start( void* arg );
			
			// non-copyable
			raster_task           ( const raster_task& );
			raster_task& operator=( const raster_task& );
		
		public:
			raster_task( const char*, poseven::mutex&, poseven::cond&, const char*, int );
			~raster_task();
			
			bool single_frame() const  { return its_sync == 0; }  // NULL
			
			bool running() const;
			
			void copy_bits() const;
			
			rect_pv16 get_rect() const;
			
			const raster::raster_desc& desc() const
			{
				return its_raster.meta->desc;
			}
			
			void send_rect( int fd, const rect_pv16& rect ) const;
	};
	
}

#endif
