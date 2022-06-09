/*
	metatype.hh
	-----------
*/

#ifndef VLIB_TYPES_STRUCT_METATYPE_HH
#define VLIB_TYPES_STRUCT_METATYPE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	extern const dispatch structmetatype_dispatch;
	
	class Struct_Metatype : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &structmetatype_dispatch;
			}
			
			Struct_Metatype() : Value( Value_other, &structmetatype_dispatch )
			{
			}
	};
	
}

#endif
