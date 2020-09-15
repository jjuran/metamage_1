/*
	metatype.hh
	-----------
*/

#ifndef VLIB_TYPES_VECTOR_METATYPE_HH
#define VLIB_TYPES_VECTOR_METATYPE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	extern const dispatch vectormetatype_dispatch;
	
	class Vector_Metatype : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &vectormetatype_dispatch;
			}
			
			Vector_Metatype() : Value( Value_other, &vectormetatype_dispatch )
			{
			}
	};
	
}

#endif
