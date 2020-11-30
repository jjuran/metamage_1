/*
	config.cc
	---------
*/

#include "config.hh"


namespace tool
{
	
	bool global_config_a4         = false;
	bool global_config_68k        = false;
	bool global_config_cfm        = false;
	bool global_config_cplusplus  = false;
	bool global_config_powerpc    = false;
	bool global_config_precompile = false;
	
	bool global_config_debug = false;
	
	char global_newline_char = '\n';
	
	line_style global_line_style = no_line_directives;
	
}
