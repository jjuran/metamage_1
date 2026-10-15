/*
	nucleus/string.hh
	-----------------
	
	Written 2010 by Joshua Juran.
	
	This code was written entirely by the above contributor, who places it
	in the public domain.
*/

#ifndef NUCLEUS_STRING_HH
#define NUCLEUS_STRING_HH

// plus
#ifndef PLUS_VARSTRING_HH
#include "plus/var_string.hh"
#endif


namespace nucleus
{
	
	typedef plus::string string;

	typedef plus::var_string mutable_string;
	
}

#endif
