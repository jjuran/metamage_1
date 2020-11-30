/*
	directive.hh
	------------
*/

#ifndef MXCPP_DIRECTIVE_HH
#define MXCPP_DIRECTIVE_HH

// plus
#include "plus/string.hh"


namespace tool
{
	
	class unclosed_macro_definition {};
	
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
