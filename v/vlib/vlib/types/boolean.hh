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
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch boolean_dispatch;
	
	class Boolean : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_boolean;
			}
			
			static Value coerce( const Value& v );
			
			Boolean( bool b = false ) : Value( V_bool, &boolean_dispatch )
			{
				pod_cast< bool >() = b;
			}
			
			operator bool() const  { return pod_cast< bool >(); }
	};
	
	extern const Boolean False;
	extern const Boolean True;
	
	extern const type_info boolean_vtype;
	
}

#endif
