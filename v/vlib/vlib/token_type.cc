/*
	token_type.cc
	-------------
*/

#include "vlib/token_type.hh"

// iota
#include "iota/char_types.hh"


#pragma exceptions off


#define ARRAY_LEN( a ) (sizeof (a) / sizeof (a)[0])
#define ARRAY_END( a ) ((a) + ARRAY_LEN(a))


namespace vlib
{
	
	using iota::is_space;
	using iota::is_digit;
	using iota::is_xdigit;
	using iota::is_alpha;
	using iota::is_alnum;
	
	
	inline bool is_initial( char c )
	{
		return iota::is_alpha( c )  ||  c == '_';
	}
	
	inline bool is_name( char c )
	{
		return iota::is_alnum( c )  ||  c == '_';
	}
	
	inline bool is_bin_digit( char c )
	{
		return c >= '0'  &&  c <= '1';
	}
	
	template < token_type type >
	struct is;
	
	template <>
	struct is< Token_bin >
	{
		static bool digit( char c )  { return is_bin_digit( c ); }
	};
	
	template <>
	struct is< Token_hex >
	{
		static bool digit( char c )  { return is_xdigit( c ); }
	};
	
	template < token_type type >
	inline
	bool scan_digits( const char*& p )
	{
		if ( ! is< type >::digit( *p++ ) )
		{
			return false;
		}
		
		while ( is< type >::digit( *p++ ) )  continue;
		
		--p;
		
		return true;
	};
	
	
	struct op_token
	{
		const char*  name;
		token_type   type;
	};
	
	static const op_token op_tokens[] =
	{
		{ "!=",  Token_bang_equals },
		
		{ "%",   Token_percent        },
		{ "%=",  Token_percent_equals },
		
		{ "&",   Token_ampersand },
		
		{ "(",   Token_lparen },
		{ "()",  Token_parens },
		{ "(*)", Token_parens_star },
		{ "(+)", Token_parens_plus },
		{ ")",   Token_rparen },
		
		{ "*",   Token_asterisk        },
		{ "*=",  Token_asterisk_equals },
		
		{ "+",   Token_plus        },
		{ "++",  Token_plus_x2     },
		{ "+=",  Token_plus_equals },
		
		{ ",",   Token_comma },
		
		{ "-",   Token_minus        },
		{ "--",  Token_minus_x2     },
		{ "-=",  Token_minus_equals },
		
		{ ".",   Token_dot    },
		{ "...", Token_dot_x3 },
		
		{ "/",   Token_slash        },
		{ "/=",  Token_slash_equals },
		
		// 0 - 9
		
		{ ":=",  Token_colon_equals },
		
		{ ";",   Token_semicolon },
		{ "<",   Token_lt        },
		{ "<=",  Token_lt_equals },
		{ "=",   Token_equals    },
		{ "==",  Token_equals_x2 },
		{ ">",   Token_gt        },
		{ ">=",  Token_gt_equals },
		
		{ "[",   Token_lbracket },
		{ "]",   Token_rbracket },
		
		{ "^",   Token_caret },
		
		{ "{",   Token_lbrace },
		{ "|",   Token_pipe   },
		{ "}",   Token_rbrace },
	};
	
	static const char* matches( const char* s, const char* pattern )
	{
		while ( *pattern != '\0' )
		{
			if ( *pattern++ != *s++ )
			{
				return 0;  // NULL
			}
		}
		
		return s;
	}
	
	static const op_token* find_op_token( const char*& p )
	{
		const op_token* begin = op_tokens;
		const op_token* end   = ARRAY_END( op_tokens );
		
		const op_token* it = end;
		
		char c = *p++;
		
		while ( it > begin )
		{
			const char* name = (--it)->name;
			
			const char nc = *name++;
			
			if ( c < nc )  continue;
			if ( c > nc )  return 0;  // NULL
			
			if ( const char* q = matches( p, name ) )
			{
				p = q;
				
				return it;
			}
		}
		
		return 0;  // NULL
	}
	
	token_type next_token_type( const char*& p )
	{
		if ( *p == '\0' )
		{
			return Token_end;
		}
		
		if ( is_space( *p ) )
		{
			do
			{
				char c = *p++;
				
				if ( c == '\n' )
				{
					return Token_newline;
				}
				
				if ( c == '\r' )
				{
					if ( *p == '\n' )  ++p;
					
					return Token_newline;
				}
			}
			while ( is_space( *p ) );
			
			return Token_whitespace;
		}
		
		if ( (signed char) *p < ' ' )
		{
			if ( (signed char) *p++ < 0 )
			{
				while ( (signed char) *p < 0 )
				{
					++p;
				}
				
				return Token_invalid;  // non-ASCII sequence
			}
			
			return Token_control;  // control character
		}
		
		if ( *p == '#' )
		{
			++p;
			
			while ( char c = *p++ )
			{
				if ( c == '\n'  ||  c == '\r' )
				{
					--p;
					
					return Token_comment;
				}
			}
			
			return Token_invalid;  // missing trailing newline
		}
		
		if ( *p == '0' )
		{
			++p;
			
			if ( ! is_alnum( *p ) )
			{
				return Token_digits;
			}
			
			char c = *p++;
			
			switch ( c )
			{
				case 'b': if ( scan_digits< Token_bin >( p ) ) return Token_bin;
				case 'x': if ( scan_digits< Token_hex >( p ) ) return Token_hex;
				
				default:
					break;
			}
			
			return Token_invalid;
		}
		
		if ( is_digit( *p ) )
		{
			while ( is_digit( *++p ) )  continue;
			
			return Token_digits;
		}
		
		if ( is_initial( *p ) )
		{
			if ( p[ 1 ] == '"' )
			{
				if ( *p == 'b' )  return ++p, Token_unbin;
				if ( *p == 'x' )  return ++p, Token_unhex;
			}
			
			while ( is_name( *++p ) )  continue;
			
			const char* q = p;
			
			while ( is_space( *q ) )  ++q;
			
			return *q == '(' ? Token_bareword_function : Token_bareword;
		}
		
		if ( *p == '\'' )
		{
			++p;
			
			while ( const char c = *p++ )
			{
				if ( c == '\'' )
				{
					return Token_byte;
				}
				
				if ( c == '\\' )
				{
					if ( *p++ == '\0' )
					{
						break;
					}
				}
			}
			
			return Token_invalid;
		}
		
		if ( *p == '"' )
		{
			++p;
			
			bool escaped = false;
			
			while ( const char c = *p++ )
			{
				if ( c == '"' )
				{
					return escaped ? Token_string_escaped : Token_string;
				}
				
				if ( c == '\\' )
				{
					if ( *p++ == '\0' )
					{
						break;
					}
					
					escaped = true;
				}
			}
			
			return Token_invalid;
		}
		
		if ( const op_token* token = find_op_token( p ) )
		{
			return token->type;
		}
		
		return Token_invalid;
	}
	
}
