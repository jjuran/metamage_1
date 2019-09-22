/*
	state.cc
	--------
*/

#include "varyx/channel/state.hh"


namespace varyx
{
namespace channel
{
	
	namespace p7 = poseven;
	
	using namespace vlib;
	
	
	void channel_state::close()
	{
		p7::lock k( its_mutex );
		
		it_is_closed = true;
		
		its_send_cond.broadcast();
		its_recv_cond.broadcast();
	}
	
	bool channel_state::send( const Value& v )
	{
		p7::lock k( its_mutex );
		
		while ( true )
		{
			if ( it_is_closed )
			{
				return false;
			}
			
			if ( its_recv_count > 0  &&  its_element.type() == 0 )  break;
			
			its_send_cond.wait( k );
		}
		
		its_element = v;
		
		its_recv_cond.signal();
		
		return true;
	}
	
	Value channel_state::recv()
	{
		p7::lock k( its_mutex );
		
		its_send_cond.signal();
		
		while ( ! it_is_closed  &&  its_element.type() == 0 )
		{
			++its_recv_count;
			
			its_recv_cond.wait( k );
			
			--its_recv_count;
		}
		
		if ( its_element.type() == 0 )
		{
			// recv on a closed channel
			return empty_list;
		}
		
		Value result;
		
		result.swap( its_element );
		
		its_send_cond.signal();
		
		return result;
	}
	
}
}
