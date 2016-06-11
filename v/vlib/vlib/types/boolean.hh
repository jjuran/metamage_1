/*
	boolean.hh
	----------
*/

#ifndef VLIB_TYPES_BOOLEAN_HH
#define VLIB_TYPES_BOOLEAN_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct type_info;
	
	class Boolean : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_boolean;
			}
			
			static Value coerce( const Value& v );
			
			Boolean( bool b = false ) : Value( b )
			{
			}
	};
	
	extern const type_info boolean_vtype;
	
}

#endif
