/*
	proc.hh
	-------
*/

#ifndef VLIB_TYPES_PROC_HH
#define VLIB_TYPES_PROC_HH

// vlib
#include "vlib/proc_info.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	extern const dispatch proc_dispatch;
	
	class Proc : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &proc_dispatch;
			}
			
			Proc( const proc_info& proc ) : Value( Value_other, &proc_dispatch )
			{
				pod_cast< const proc_info* >() = &proc;
				
				set_cycle_free();
			}
			
			function_type addr() const
			{
				return dereference< proc_info >().addr;
			}
			
			Value call( const Value& arguments ) const
			{
				return addr()( arguments );
			}
	};
	
}

#endif
