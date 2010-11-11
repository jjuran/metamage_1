/*
	derived_filename.hh
	-------------------
*/

#ifndef ALINE_DERIVEDFILENAME_HH
#define ALINE_DERIVEDFILENAME_HH

// plus
#include "plus/string.hh"


namespace tool
{
	
	plus::string derived_filename( const plus::string& source_path );
	
	plus::string derived_pathname( const plus::string&  dir,
	                               const plus::string&  source_path,
	                               const char*          extension );
	
}

#endif

