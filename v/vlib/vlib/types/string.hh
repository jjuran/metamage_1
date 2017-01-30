/*
	string.hh
	---------
*/

#ifndef VLIB_TYPES_STRING_HH
#define VLIB_TYPES_STRING_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct proc_info;
	struct type_info;
	
	class String : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_string;
			}
			
			static Value coerce( const Value& v );
			
			String( const plus::string& s = plus::string::null )
			:
				Value( (const vu_string&) s, V_str )
			{
			}
	};
	
	extern const type_info string_vtype;
	
	extern const proc_info proc_join;
	
}

#endif
