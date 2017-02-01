/*
	vector.hh
	---------
*/

#ifndef VLIB_TYPES_VECTOR_HH
#define VLIB_TYPES_VECTOR_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct type_info;
	
	class Vector : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_vector;
			}
			
			static Value coerce( const Value& v );
			
			Vector( const plus::string& s = plus::string::null )
			:
				Value( (const vu_string&) s, V_vec )
			{
			}
	};
	
	extern const type_info vector_vtype;
	
}

#endif
