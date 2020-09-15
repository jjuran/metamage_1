/*
	record.hh
	---------
*/

#ifndef VLIB_TYPES_RECORD_HH
#define VLIB_TYPES_RECORD_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch record_dispatch;
	
	class Record : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &record_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			explicit Record( const Value& fields );
	};
	
	extern const type_info record_vtype;
	
}

#endif
