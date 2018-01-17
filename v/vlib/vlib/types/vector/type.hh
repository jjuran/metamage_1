/*
	type.hh
	-------
*/

#ifndef VLIB_TYPES_VECTOR_TYPE_HH
#define VLIB_TYPES_VECTOR_TYPE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	extern const dispatch vectortype_dispatch;
	
	class Vector_Type : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &vectortype_dispatch;
			}
			
			explicit Vector_Type( const Value& element_type );
			
			const Value& element_type() const;
	};
	
}

#endif
