/*
	deconstruct_char.hh
	-------------------
*/

#ifndef PLUS_DECONSTRUCTCHAR_HH
#define PLUS_DECONSTRUCTCHAR_HH

// plus
#include "plus/var_string.hh"


namespace plus
{
	
	struct deconstruct_char
	{
		static void apply( plus::var_string& out, char data, bool binary )
		{
			if ( data != '\0'  ||  binary )
			{
				out += data;
			}
		}
	};
	
}

#endif

