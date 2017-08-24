/*
	pointer.hh
	----------
*/

#ifndef VLIB_TYPES_POINTER_HH
#define VLIB_TYPES_POINTER_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch pointer_dispatch;
	
	class Pointer : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &pointer_dispatch;
			}
			
			Pointer( const Value& container );
	};
	
	extern const type_info pointer_vtype;
	
}

#endif
