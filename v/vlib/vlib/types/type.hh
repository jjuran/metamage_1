/*
	type.hh
	-------
*/

#ifndef VLIB_TYPES_TYPE_HH
#define VLIB_TYPES_TYPE_HH

// vlib
#include "vlib/is_type.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch type_dispatch;
	
	class Type : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return is_type( v );
			}
			
			Type( const type_info& type ) : Value( type, &type_dispatch )
			{
			}
	};
	
	extern const type_info type_vtype;
	
}

#endif
