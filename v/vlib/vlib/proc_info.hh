/*
	proc_info.hh
	------------
*/

#ifndef VLIB_PROCINFO_HH
#define VLIB_PROCINFO_HH


namespace vlib
{
	
	class Value;
	
	typedef Value (*function_type)( const Value& argument );
	
	struct proc_info
	{
		function_type const  addr;
		char const* const    name;
		const Value* const   prototype;
	};
	
}

#endif
