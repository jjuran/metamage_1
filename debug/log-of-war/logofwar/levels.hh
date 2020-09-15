/*
	levels.hh
	---------
*/

#ifndef LOGOFWAR_LEVELS_HH
#define LOGOFWAR_LEVELS_HH

#ifndef CONFIG_DEBUGGING
#define CONFIG_DEBUGGING  0
#endif

namespace logofwar
{
	
	enum diagnostic_level
	{
		Level_fatal,
		Level_error,
		Level_warning,
		Level_notice,
		Level_info,
		Level_debug,
	};
	
	const diagnostic_level maximum_verbosity = CONFIG_DEBUGGING ? Level_debug
	                                                            : Level_warning;
	
	extern diagnostic_level current_verbosity;
	
	extern char const* const diagnostic_level_names[];
	
}

#endif
