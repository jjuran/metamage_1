/*
	null.hh
	-------
*/

#ifndef VLIB_TYPES_NULL_HH
#define VLIB_TYPES_NULL_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct type_info;
	
	extern const type_info null_vtype;
	
	class Null : public Value
	{
		public:
			static bool test( const Value& v );
			
			static Value coerce( const Value& v );
			
			Null() : Value( null_vtype )
			{
			}
	};
	
	inline
	bool Null::test( const Value& v )
	{
		return v.type() == Value_base_type  &&  &v.typeinfo() == &null_vtype;
	}
	
}

#endif
