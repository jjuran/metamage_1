/*
	channel.hh
	----------
*/

#ifndef CHANNEL_CHANNEL_HH
#define CHANNEL_CHANNEL_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class channel_state;
	
	struct dispatch;
	
	extern const dispatch channel_dispatch;
	
	class Channel : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &channel_dispatch;
			}
			
			Channel();
			
			channel_state* get() const
			{
				return (channel_state*) pointer();
			}
	};
	
}

#endif
