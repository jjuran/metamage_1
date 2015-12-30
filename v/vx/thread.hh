/*
	thread.hh
	---------
*/

#ifndef THREAD_HH
#define THREAD_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class thread_state;
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch thread_dispatch;
	
	class Thread : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &thread_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			Thread( const Value& entry_point );
			
			thread_state* get() const
			{
				return (thread_state*) pointer();
			}
	};
	
	extern const type_info thread_vtype;
	
}

#endif
