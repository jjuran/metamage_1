/*
	expression.hh
	-------------
*/

#ifndef MXCPP_EXPRESSION_HH
#define MXCPP_EXPRESSION_HH

// plus
#include "plus/string.hh"


namespace tool
{
	
	class bad_integer_constant {};
	class bad_preprocessor_constant {};
	class bad_preprocessor_atom {};
	class bad_preprocessor_op {};
	
	bool expression_is_true( const plus::string& expression );
	
}

#endif
