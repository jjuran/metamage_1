/*
	concept.hh
	----------
*/

#ifndef CHANNEL_CONCEPT_HH
#define CHANNEL_CONCEPT_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	extern const dispatch channelconcept_dispatch;
	
	class Channel_Concept : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &channelconcept_dispatch;
			}
			
			Channel_Concept() : Value( Value_other, &channelconcept_dispatch )
			{
			}
	};
	
}

#endif
