/*
	send.hh
	-------
*/

#ifndef SEND_HH
#define SEND_HH

// poseven
#include "poseven/types/cond.hh"
#include "poseven/types/thread.hh"

// rasterlib
#include "raster/raster.hh"


namespace telekine
{
	
	class send_task
	{
		private:
			poseven::mutex&      its_ready_mutex;
			poseven::cond&       its_ready_cond;
			poseven::thread      its_thread;
			int                  its_in_fd;
			int                  its_out_fd;
			
			void ready();
			
			static void* wait_start( void* arg );
			
			// non-copyable
			send_task           ( const send_task& );
			send_task& operator=( const send_task& );
		
		public:
			send_task( int in, int out, poseven::mutex&, poseven::cond& );
			~send_task();
			
			void send_desc( const raster::raster_desc& desc ) const;
	};
	
}

#endif
