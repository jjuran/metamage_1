/*
	include.hh
	----------
*/

#ifndef MXCPP_INCLUDE_HH
#define MXCPP_INCLUDE_HH

// plus
#include "plus/string_fwd.hh"

// vxo
#include "vxo/ptrvec.hh"


typedef vxo::UniPtrVec_< const char > string_constant_vector;

namespace tool
{
	
	class unterminated_include_path {};
	
	extern string_constant_vector global_include_search_paths;
	
	void mark_current_source_once_included();
	
	void memoize_include_guard( const plus::string& file, const plus::string& guard );
	
	void include_file( const plus::string& target );
	
}

#endif
