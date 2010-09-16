/*
	deconstruct_bool.hh
	-------------------
*/

#ifndef PLUS_DECONSTRUCTBOOL_HH
#define PLUS_DECONSTRUCTBOOL_HH

// plus
#include "plus/var_string.hh"


namespace plus
{
	
	struct deconstruct_bool
	{
		static void apply( plus::var_string& out, bool data, bool binary )
		{
			out += '0' * !binary + data;
		}
	};
	
}

#endif

