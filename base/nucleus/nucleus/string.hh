/*
	nucleus/string.hh
	-----------------
	
	Written 2010 by Joshua Juran.
	
	This code was written entirely by the above contributor, who places it
	in the public domain.
*/

#ifndef NUCLEUS_STRING_HH
#define NUCLEUS_STRING_HH


#ifdef NUCLEUS_STRING_HEADER
#include NUCLEUS_STRING_HEADER
#elif defined( __ALINE__ )
#include "plus/var_string.hh"
#else
#include <string>
#endif


namespace nucleus
{
#ifdef NUCLEUS_STRING_TYPE
	
	typedef NUCLEUS_STRING_TYPE string;
	
#elif defined( __ALINE__ )
	
	typedef plus::string string;
	
#else
	
	typedef std::string string;
	
#endif

#ifdef NUCLEUS_MUTABLE_STRING_TYPE
	
	typedef NUCLEUS_MUTABLE_STRING_TYPE mutable_string;
	
#elif defined NUCLEUS_STRING_TYPE
	
	typedef NUCLEUS_STRING_TYPE mutable_string;
	
#elif defined( __ALINE__ )
	
	typedef plus::var_string mutable_string;
	
#else
	
	typedef std::string mutable_string;
	
#endif
}

#endif

