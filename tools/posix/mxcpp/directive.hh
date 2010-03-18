/*
	directive.hh
	------------
*/

#ifndef MXCPP_DIRECTIVE_HH
#define MXCPP_DIRECTIVE_HH

// Standard C/C++
#include <cctype>

// plus
#include "plus/string.hh"


namespace tool
{
	
	class unclosed_macro_definition {};
	
	inline bool is_space( char c )
	{
		return std::isspace( c );
	}
	
	bool emitting();
	
	struct directive
	{
		typedef bool (*d_handler_t)( const plus::string&, const plus::string& );
		typedef const char* (*scanner_t)( const char*, const char* );
		
		const char*  name;
		d_handler_t  code;
		scanner_t    scan;
	};
	
	const directive* find_directive( const plus::string& name );
	
}

#endif

