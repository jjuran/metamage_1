/*
	pragma.hh
	---------
*/

#ifndef MXCPP_PRAGMA_HH
#define MXCPP_PRAGMA_HH

// Standard C++
#include <exception>

// plus
#include "plus/string_fwd.hh"


namespace tool
{
	
	class illegal_pragma : public std::exception
	{
		const char* what() const throw()
		{
			return "Illegal pragma";
		}
	};
	
	class illegal_pragma_param : public std::exception
	{
		const char* what() const throw()
		{
			return "Illegal pragma parameter";
		}
	};
	
	bool check_option( const plus::string& name );
	
	bool pragma( const plus::string& name, const plus::string& param );
	
}

#endif
