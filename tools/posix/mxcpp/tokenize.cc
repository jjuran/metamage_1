/*
	tokenize.cc
	-----------
*/

#include "tokenize.hh"

// Standard C
#include <ctype.h>
#include <string.h>

// mxcpp
#include "op_token.hh"


namespace tool
{
	
	static bool next_token( const char*& p )
	{
		while ( isspace( *p ) )
		{
			++p;
		}
		
		return *p != '\0';
	}
	
	static inline bool is_identifier( char c )
	{
		return isalnum( c )  ||  c == '_';
	}
	
	static inline bool is_exponent( char c )
	{
		c |= ' ';
		
		return c == 'e'  ||  c == 'p';
	}
	
	static bool is_number_char( const char* p )
	{
		switch ( *p )
		{
			case '.':
			case '_':
				return true;
			
			case '+':
			case '-':
				return is_exponent( p[ -1 ] );
			
			default:
				return isalnum( *p );
		}
	}
	
	static void skip_number( const char*& p )
	{
		while ( is_number_char( ++p ) )
		{
			continue;
		}
	}
	
	static inline void skip_string_literal( const char*& p )
	{
		const char c = *p;
		
		while ( *++p != c )
		{
			if ( *p == '\\' )
			{
				++p;
			};
		}
		
		++p;
	}
	
	static void skip_identifier( const char*& p )
	{
		while ( is_identifier( *++p ) ) continue;
	}
	
	
	void tokenize( const plus::string& input, token_list& output )
	{
		const char* p = input.c_str();
		
		while ( next_token( p ) )
		{
			const char* q = p;
			
			const bool dot = *p == '.';
			
			if ( isdigit( p[ dot ] ) )
			{
				skip_number( q );
			}
			else if ( *p == '\''  ||  *p == '"' )
			{
				skip_string_literal( q );
			}
			else if ( is_identifier( *p ) )
			{
				if ( p[0] == 'L'  &&  (p[1] == '\'' || p[1] == '"') )
				{
					skip_string_literal( ++q );
				}
				else
				{
					skip_identifier( q );
				}
			}
			else if ( const char* op = find_op_token( p ) )
			{
				q = p + strlen( op );
			}
			else
			{
				q = p + 1;
			}
			
			plus::string token( p, q );
			
			p = q;
			
			output.push_back( token );
		}
	}
	
	token_list tokenize( const plus::string& input )
	{
		token_list result;
		
		tokenize( input, result );
		
		return result;
	}
	
}
