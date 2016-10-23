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
			
			Vector() : Value( plus::string::null, vector_tag() )
			{
			}
			
			Vector( const plus::string& s ) : Value( s, vector_tag() )
			{
			}
	};
	
	extern const type_info vector_vtype;
	
}

#endif
