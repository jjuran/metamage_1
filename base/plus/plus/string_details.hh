/*
	string_details.hh
	-----------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef PLUS_STRINGDETAILS_HH
#define PLUS_STRINGDETAILS_HH

// plus
#include "plus/string.hh"


namespace plus
{
	
	inline string::size_type adjusted_capacity( string::size_type capacity )
	{
		const int n_missing_bits_of_precision = 2;
		
		return capacity | (1 << n_missing_bits_of_precision) - 1;
	}
	
}

#endif

