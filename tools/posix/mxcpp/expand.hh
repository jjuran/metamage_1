/*
	expand.hh
	---------
*/

#ifndef MXCPP_EXPAND_HH
#define MXCPP_EXPAND_HH


namespace tool
{
	
	class token_list;
	
	class pseudomacro_got_multiple_args {};
	class pseudomacro_got_multiple_tokens {};
	class pseudomacro_got_no_tokens {};
	
	bool expand_macros( const token_list& input, token_list& output, bool in_expression );
	
}

#endif
