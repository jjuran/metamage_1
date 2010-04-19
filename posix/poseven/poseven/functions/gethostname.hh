/*
	gethostname.hh
	--------------
	
	Copyright 2009, Joshua Juran.
*/

#ifndef POSEVEN_FUNCTIONS_GETHOSTNAME_HH
#define POSEVEN_FUNCTIONS_GETHOSTNAME_HH

// Standard C++
#include <string>


namespace poseven
{
	
	void gethostname( std::string& name );
	
	const char* gethostname();
	
}

#endif

