/*
	tokenize.hh
	-----------
*/

#ifndef MXCPP_TOKENIZE_HH
#define MXCPP_TOKENIZE_HH

// plus
#include "plus/string.hh"

// mxcpp
#include "token_list.hh"


namespace tool
{
	
	void tokenize( const plus::string& input, token_list& output );
	
	token_list tokenize( const plus::string& input );
	
}

#endif
