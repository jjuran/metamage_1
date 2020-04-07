/*
	thread_state.hh
	---------------
*/

#ifndef VARYX_POSIX_THREADSTATE_HH
#define VARYX_POSIX_THREADSTATE_HH

// poseven
#include "poseven/types/thread.hh"

// vlib
#include "vlib/value.hh"


namespace varyx
{
namespace posix
{
	
	using vlib::Value;
	
	enum thread_status
	{
		Thread_running,
		Thread_returned,
		Thread_cancelled,
		Thread_threw_exception,
	};
	
	struct thread_parameter_block
	{
		Value f;
		Value result;
		
		thread_status status;
	};
	
	class thread_state
	{
		private:
			poseven::thread         its_thread;
			thread_parameter_block  its_pb;
			
			// non-copyable
			thread_state           ( const thread_state& );
			thread_state& operator=( const thread_state& );
		
		public:
			static void* pthread_start( void* param );
			
			thread_state( const Value& f );
			
			~thread_state()
			{
				join();
			}
			
			void cancel()  { its_thread.cancel(); }
			
			void join();
			
			const Value& operator*();
			
			const Value& function() const  { return its_pb.f;      }
			const Value& result  () const  { return its_pb.result; }
	};
	
	void join_all_threads();
	
}
}

#endif
