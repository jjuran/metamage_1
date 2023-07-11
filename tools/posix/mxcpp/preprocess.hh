/*
	preprocess.hh
	-------------
*/

#ifndef MXCPP_PREPROCESS_HH
#define MXCPP_PREPROCESS_HH

// plus
#include "plus/string_fwd.hh"


namespace tool
{
	
	const plus::string& get_current_source_path();
	
	const plus::string& get_current_FILE();
	
	unsigned get_current_LINE();
	
	void set_line( const plus::string& line, const plus::string& file );
	
	void preprocess_file( const char *dir_path, int dirfd, const plus::string& include_path );
	
	void preprocess_file( const char *path );
	
}

#endif
