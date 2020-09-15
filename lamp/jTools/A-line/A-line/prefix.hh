/*
	prefix.hh
	---------
*/

#ifndef ALINE_PREFIX_HH
#define ALINE_PREFIX_HH

// A-line
#include "A-line/Platform.hh"


namespace tool
{
	
	class Project;
	
	const Project* get_project_providing_prefix( const Project&  project,
	                                             Platform        platform );
	
}

#endif
