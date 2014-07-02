/*
	FSSpec_from_stat.hh
	-------------------
*/

#ifndef MACRELIX_FSSPECFROMSTAT_HH
#define MACRELIX_FSSPECFROMSTAT_HH


struct FSSpec;
struct stat;


namespace mac   {
namespace relix {
	
	int FSSpec_from_stat( const struct stat& stat_buffer, FSSpec& result );
	
}
}

#endif
