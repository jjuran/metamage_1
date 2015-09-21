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
		Token_equals_x2,
		Token_bang_equals,
		Token_lt,
		Token_lt_equals,
		Token_gt,
		Token_gt_equals,
	};
	
	token_type next_token_type( const char*& p );
	
}

#endif
