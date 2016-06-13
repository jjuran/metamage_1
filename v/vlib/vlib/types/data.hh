/*
	data.hh
	-------
*/

#ifndef VLIB_TYPES_DATA_HH
#define VLIB_TYPES_DATA_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct type_info;
	
	class Data : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_data;
			}
			
			static Value coerce( const Value& v );
			
			Data() : Value( plus::string::null, data_tag() )
			{
			}
			
			Data( const plus::string& s ) : Value( s, data_tag() )
			{
			}
	};
	
	extern const type_info data_vtype;
	
}

#endif
