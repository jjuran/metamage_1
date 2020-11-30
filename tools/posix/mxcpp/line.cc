/*
	line.cc
	-------
*/

#include "line.hh"

// Standard C
#include <ctype.h>

// gear
#include "gear/find.hh"

// plus
#include "plus/var_string.hh"

// mxcpp
#include "config.hh"
#include "directive.hh"
#include "expand.hh"
#include "tokenize.hh"


namespace tool
{
	
	static const unsigned char* whitespace = (const unsigned char*) "\4 \t\n\r";
	
	static const char* skip_space( const char* begin, const char* end )
	{
		return gear::find_first_nonmatch( begin, end - begin, whitespace, end );
	}
	
	static inline char* skip_space( char* begin, const char* end )
	{
		return (char*) skip_space( (const char*) begin, end );
	}
	
	static const char* skip_alpha( const char* begin, const char* end )
	{
		while ( begin < end )
		{
			const char c = *begin++ | ' ';
			
			if ( c < 'a'  ||  c > 'z' )
			{
				return begin - 1;
			}
		}
		
		return end;
	}
	
	
	static plus::string join_tokens( const token_list& tokens )
	{
		plus::var_string result;
		
		const std::size_t n_tokens = tokens.get().size();
		
		for ( int i = 0;  i < n_tokens;  ++i )
		{
			const plus::string& token = tokens.get()[ i ];
			
			result += token;
			result += " ";
		}
		
		if ( !result.empty() )
		{
			*(result.end() - 1) = global_newline_char;
		}
		
		return result;
	}
	
	static bool expand_macros( plus::string& line, bool in_expression )
	{
		token_list result_tokens;
		
		if ( expand_macros( tokenize( line ), result_tokens, in_expression ) )
		{
			line = join_tokens( result_tokens );
			
			return true;
		}
		
		return false;
	}
	
	
	bool process_line( plus::string& line )
	{
		const char* p = &line[0];
		
		const char* q = p + line.size();
		
		p = skip_space( p, q - 1 );
		
		if ( *p == '#' )
		{
			while ( q > p  &&  isspace( q[ -1 ] ) )
			{
				--q;
			}
			
			const char* directive_start = skip_space( p + 1, q );
			
			const char* word_end = skip_alpha( directive_start, q );
			
			plus::string name( directive_start, word_end );
			
			if ( const directive* d = find_directive( name ) )
			{
				const char* target_start = skip_space( word_end, q );
				
				const char* mid = d->scan( target_start, q );
				
				plus::string target( target_start, mid );
				
				const char* param_start = skip_space( mid, q );
				
				plus::string param( param_start, q );
				
				const bool print = d->code( target, param )  &&  emitting();
				
				if ( !print )
				{
					line = "";
				}
			}
		}
		else if ( emitting() )
		{
			const bool in_expression_or_not = false;
			
			return expand_macros( line, in_expression_or_not );
		}
		else
		{
			line = "";
		}
		
		return true;
	}
	
}
