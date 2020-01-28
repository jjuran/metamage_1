/*
	table.hh
	--------
*/

#ifndef VLIB_TYPES_TABLE_HH
#define VLIB_TYPES_TABLE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	extern const dispatch table_dispatch;
	
	bool is_table( const Value& v );
	
	class Table : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &table_dispatch;
			}
			
			Table( const Value& key_type, const Value& array );
	};
	
}

#endif
