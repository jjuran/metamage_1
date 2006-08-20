/*	================
 *	StringFilters.hh
 *	================
 *	
 *	No implementation necessary.
 */

#pragma once

// Standard C++
#include <string>


namespace BitsAndBytes {
	
	using std::string;
	
	inline string q(const string& str)
	{
		return string("'") + str + "'";
	}
	
	inline string qq(const string& str)
	{
		return string("\"") + str + '"';
	}
	
	inline string uq(const string& str)
	{
		return string("`") + str + "'";
	}
	
}

