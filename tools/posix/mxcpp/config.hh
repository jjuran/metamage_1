/*
	config.hh
	---------
*/

#ifndef MXCPP_CONFIG_HH
#define MXCPP_CONFIG_HH


namespace tool
{
	
	extern bool global_config_a4;
	extern bool global_config_68k;
	extern bool global_config_cfm;
	extern bool global_config_cplusplus;
	extern bool global_config_powerpc;
	extern bool global_config_precompile;
	
	extern bool global_config_debug;
	
	extern char global_newline_char;
	
	enum line_style
	{
		no_line_directives,
		unix_line_directives,
		mac_line_directives
	};
	
	extern line_style global_line_style;
	
}

#endif
