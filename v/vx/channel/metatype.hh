/*
	metatype.hh
	-----------
*/

#ifndef CHANNEL_METATYPE_HH
#define CHANNEL_METATYPE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	extern const dispatch channelmetatype_dispatch;
	
	class Channel_Metatype : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &channelmetatype_dispatch;
			}
			
			Channel_Metatype() : Value( Value_other, &channelmetatype_dispatch )
			{
			}
	};
	
}

#endif
