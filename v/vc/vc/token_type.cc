/*
	token_type.cc
	-------------
*/

#include "vc/token_type.hh"

// iota
#include "iota/char_types.hh"


#define ARRAY_LEN( a ) (sizeof (a) / sizeof (a)[0])
#define ARRAY_END( a ) ((a) + ARRAY_LEN(a))


namespace vc
{
	
	using iota::is_space;
	using iota::is_digit;
	using iota::is_alpha;
	using iota::is_alnum;
	
	
	struct op_token
	{
		const char*  name;
		token_type   type;
	};
	
	static const op_token op_tokens[] =
	{
		{ "!=",  Token_bang_equals },
		
		{ "%",   Token_percent },
		
		{ "(",   Token_lparen },
		{ "()",  Token_parens },
		{ ")",   Token_rparen },
		
		{ "*",   Token_asterisk },
		
		{ "+",   Token_plus },
		{ ",",   Token_comma },
		{ "-",   Token_minus },
		
		{ "/",   Token_slash },
		
		{ ";",   Token_semicolon },
		{ "<",   Token_lt        },
		{ "<=",  Token_lt_equals },
		{ "=",   Token_equals    },
		{ "==",  Token_equals_x2 },
		{ ">",   Token_gt        },
		{ ">=",  Token_gt_equals },
		
		{ "^",   Token_caret },
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
			while ( is_space( *++p ) )  continue;
			
			return Token_whitespace;
		}
		
		if ( *p < ' ' )
		{
			return Token_invalid;  // control character
		}
		
		if ( is_digit( *p ) )
		{
			while ( is_digit( *++p ) )  continue;
			
			return Token_digits;
		}
		
		if ( is_alpha( *p ) )
		{
			while ( is_alnum( *++p ) )  continue;
			
			return Token_bareword;
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
