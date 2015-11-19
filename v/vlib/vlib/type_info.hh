/*
	type_info.hh
	------------
*/

#ifndef VLIB_TYPEINFO_HH
#define VLIB_TYPEINFO_HH


namespace vlib
{
	
	class Value;
	
	typedef Value (*assign_proc)( const Value& v );
	typedef Value (*coerce_proc)( const Value& v );
	
	struct type_info
	{
		char const* const  name;
		assign_proc const  assign;
		coerce_proc const  coerce;
	};
	
}

#endif
