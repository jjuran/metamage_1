/*
	levels.cc
	---------
*/

#include "logofwar/levels.hh"


namespace logofwar
{

diagnostic_level current_verbosity = CONFIG_DEBUGGING ? Level_notice
                                                      : Level_warning;

char const* const diagnostic_level_names[] =
{
	"FATAL",
	"ERROR",
	"WARNING",
	"NOTICE",
	"INFO",
	"DEBUG",
};

}
