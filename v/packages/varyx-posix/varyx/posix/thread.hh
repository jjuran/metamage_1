/*
	thread.hh
	---------
*/

#ifndef VARYX_POSIX_THREAD_HH
#define VARYX_POSIX_THREAD_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
}

namespace varyx
{
namespace posix
{
	
	using vlib::dispatch;
	using vlib::type_info;
	using vlib::Value;
	
	class thread_state;
	
	extern const dispatch thread_dispatch;
	
	class Thread : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &thread_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			explicit Thread( const Value& entry_point );
			
			thread_state* get() const
			{
				return (thread_state*) pointer();
			}
	};
	
	extern const type_info thread_vtype;
	
}
}

#endif
