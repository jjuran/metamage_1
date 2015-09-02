/*
	token_type.hh
	-------------
*/

#ifndef VC_TOKENTYPE_HH
#define VC_TOKENTYPE_HH


namespace vc
{
	
	enum token_type
	{
		Token_invalid = -1,
		
		Token_end = 0,
		
		Token_whitespace,
		Token_digits,
		Token_bareword,
		Token_lparen,
		Token_rparen,
		Token_plus,
		Token_minus,
		Token_asterisk,
		Token_slash,
		Token_percent,
		Token_caret,
	};
	
	token_type next_token_type( const char*& p );
	
}

#endif
