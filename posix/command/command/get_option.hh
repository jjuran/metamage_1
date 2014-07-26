/*
	get_option.hh
	-------------
*/

#ifndef COMMAND_GETOPTION_HH
#define COMMAND_GETOPTION_HH


namespace command
{
	
	namespace constants
	{
		
		enum
		{
			Option_undefined     = -1,
			Option_param_missing = -2,
			Option_param_bogus   = -3,
		};
		
		enum
		{
			Param_unwanted,
			Param_required,
			Param_optional,
		};
		
	}
	
	using namespace constants;
	
	struct option
	{
		const char*  name;
		short        code;
		short        mode;  // parameter mode
	};
	
	struct option_result
	{
		const char*  param;
		const char*  mark;
	};
	
	extern option_result global_result;
	
	short get_option_( char* const*& argv, const option* options, option_result& result );
	
	inline short get_option( char* const**   argvp,
	                         const option*   options,
	                         option_result*  result = &global_result )
	{
		if ( argvp == 0 )  return 0;  // NULL (argvp)
		
		return get_option_( *argvp, options, *result );
	}
	
}

#endif
