/*
	namespace.hh
	------------
*/

#ifndef VLIB_TYPES_NAMESPACE_HH
#define VLIB_TYPES_NAMESPACE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct namespace_info;
	
	extern const dispatch namespace_dispatch;
	
	class Namespace : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &namespace_dispatch;
			}
			
			Namespace( const namespace_info& name_space )
			:
				Value( Value_other, &namespace_dispatch )
			{
				pod_cast< const namespace_info* >() = &name_space;
			}
			
			const namespace_info& get() const
			{
				return *pod_cast< const namespace_info* >();
			}
			
			const namespace_info& operator* () const  { return  get(); }
			const namespace_info* operator->() const  { return &get(); }
	};
	
}

#endif
