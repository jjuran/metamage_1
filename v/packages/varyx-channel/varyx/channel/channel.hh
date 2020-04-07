/*
	channel.hh
	----------
*/

#ifndef VARYX_CHANNEL_CHANNEL_HH
#define VARYX_CHANNEL_CHANNEL_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
}

namespace varyx
{
namespace channel
{
	
	using vlib::dispatch;
	using vlib::Value;
	
	class channel_state;
	
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
}

#endif
