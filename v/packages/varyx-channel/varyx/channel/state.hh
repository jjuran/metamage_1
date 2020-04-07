/*
	state.hh
	--------
*/

#ifndef VARYX_CHANNEL_STATE_HH
#define VARYX_CHANNEL_STATE_HH

// poseven
#include "poseven/types/cond.hh"
#include "poseven/types/mutex.hh"

// vlib
#include "vlib/value.hh"


namespace varyx
{
namespace channel
{
	
	using vlib::Value;
	
	class channel_state
	{
		private:
			poseven::mutex  its_mutex;
			poseven::cond   its_send_cond;
			poseven::cond   its_recv_cond;
			bool            it_is_closed;
			unsigned        its_recv_count;
			Value           its_element;
			
			// non-copyable
			channel_state           ( const channel_state& );
			channel_state& operator=( const channel_state& );
		
		public:
			channel_state() : it_is_closed(), its_recv_count()
			{
			}
			
			void close();
			
			bool send( const Value& v );
			
			Value recv();
	};
	
}
}

#endif
