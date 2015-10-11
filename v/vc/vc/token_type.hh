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
		Token_string,
		Token_string_escaped,
		Token_bareword,
		Token_bareword_function,
		Token_lparen,
		Token_rparen,
		Token_parens,
		Token_parens_star,
		Token_parens_plus,
		Token_dot,
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
		Token_comma,
		Token_equals,
		Token_plus_equals,
		Token_minus_equals,
		Token_asterisk_equals,
		Token_slash_equals,
		Token_percent_equals,
		Token_semicolon,
	};
	
	token_type next_token_type( const char*& p );
	
}

#endif
