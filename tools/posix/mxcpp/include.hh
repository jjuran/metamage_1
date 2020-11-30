/*
	include.hh
	----------
*/

#ifndef MXCPP_INCLUDE_HH
#define MXCPP_INCLUDE_HH

// Standard C++
#include <vector>

// plus
#include "plus/string_fwd.hh"


namespace tool
{
	
	class unterminated_include_path {};
	
	extern std::vector< const char* > global_include_search_paths;
	
	void mark_current_source_once_included();
	
	void memoize_include_guard( const plus::string& file, const plus::string& guard );
	
	void include_file( const plus::string& target );
	
}

#endif
