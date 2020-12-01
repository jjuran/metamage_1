/*
	expand.cc
	---------
*/

#include "expand.hh"

// Standard C
#include <ctype.h>

// plus
#include "plus/var_string.hh"

// vxo
#include "vxo/strmap.hh"
#include "vxo/strset.hh"

// mxcpp
#include "exception.hh"
#include "macro.hh"
#include "pragma.hh"
#include "predefined.hh"


typedef vxo::StrMap_to< token_list > token_list_map;

namespace tool
{
	
	using std::size_t;
	
	
	static inline bool is_initial( char c )
	{
		return isalpha( c )  ||  c == '_';
	}
	
	
	static plus::string get_one_token( const token_lists& args )
	{
		if ( args.size() != 1 )
		{
			throw exception( "pseudomacro_got_multiple_args" );
		}
		
		const token_list& tokens = args[0];
		
		if ( tokens.size() != 1 )
		{
			if ( tokens.empty() )
			{
				throw exception( "pseudomacro_got_no_tokens" );
			}
			else
			{
				throw exception( "pseudomacro_got_multiple_tokens" );
			}
		}
		
		return tokens[0];
	}
	
	static void add_boolean_token( bool boolean, token_list& result )
	{
		const char c = '0' + boolean;
		
		result.push_back( plus::string( &c, 1 ) );
	}
	
	static bool get_macro_args( const token_list& tokens, token_lists& result, size_t& i )
	{
		result.push_back( token_list() );
		
		const size_t n_tokens = tokens.size();
		
		++i;
		
		for ( int depth = 0;  i < n_tokens;  ++i )
		{
			const plus::string& token = tokens[ i ];
			
			if ( token.size() == 1 )
			{
				switch ( token[ 0 ] )
				{
					case '(':
						++depth;
						break;
					
					case ')':
						if ( depth == 0 )
						{
							return true;
						}
						
						--depth;
						break;
					
					case ',':
						if ( depth == 0 )
						{
							result.push_back( token_list() );
							continue;
						}
						
						break;
					
					default:
						break;
				}
			}
			
			result.back().push_back( token );
		}
		
		return false;
	}
	
	
	static token_list_map map_args( const token_list&   pattern,
	                                const token_lists&  arg_list )
	{
		token_list_map result;
		
		const size_t n_tokens = pattern.size();
		
		size_t n_args = 0;
		
		// skip macro name and '('
		
		for ( size_t i = 2;  i < n_tokens;  ++i )
		{
			const plus::string& token = pattern[ i ];
			
			if ( token == ")" )
			{
				break;
			}
			
			if ( token == "," )
			{
				continue;
			}
			
			token_list& arg_tokens = result[ token ];
			
			if ( n_args < arg_list.size() )
			{
				arg_tokens = arg_list[ n_args++ ];
			}
		}
		
		return result;
	}
	
	static plus::string escape_string_literal( const plus::string& s )
	{
		plus::var_string result;
		
		const char* begin = s.c_str();
		
		const char* quote = begin + (begin[0] == 'L');
		
		for ( const char* p = quote;  *p != '\0';  ++p )
		{
			if ( *p == '\\'  ||  *p == *quote )
			{
				result += '\\';
			}
			
			result += *p;
		}
		
		return result;
	}
	
	static bool is_string_literal( const char* p )
	{
		if ( *p == 'L' )
		{
			++p;
		}
		
		return *p == '\''  ||  *p == '"';
	}
	
	static plus::string stringify_tokens( const token_list& tokens )
	{
		plus::var_string result;
		
		result = '"';
		
		const size_t n_tokens = tokens.size();
		
		for ( int i = 0;  i < n_tokens;  ++i )
		{
			const plus::string& token = tokens[ i ];
			
			if ( is_string_literal( token.c_str() ) )
			{
				result.append( escape_string_literal( token ) );
			}
			else
			{
				result.append( token.data(), token.size() );
			}
			
			result += " ";
		}
		
		if ( n_tokens )
		{
			result.resize( result.size() - 1 );
		}
		
		result += '"';
		
		return result;
	}
	
	static void substitute_args( const token_list&      tokens,
	                             const token_list_map&  args,
	                             token_list&            result )
	{
		const size_t n_tokens = tokens.size();
		
		bool preceded_by_hash = false;
		
		for ( int i = 0;  i < n_tokens;  ++i )
		{
			const plus::string& token = tokens[ i ];
			
			if ( token == "#" )
			{
				preceded_by_hash = true;
				
				continue;
			}
			
			token_list_map::const_iterator it = args.find( token );
			
			if ( it != args.end() )
			{
				const token_list& arg_tokens = it->second;
				
				if ( preceded_by_hash )
				{
					plus::string stringified = stringify_tokens( arg_tokens );
					
					result.push_back( stringified );
				}
				else
				{
					result.insert( result.end(),
					               arg_tokens.begin(),
					               arg_tokens.end() );
				}
			}
			else
			{
				if ( preceded_by_hash )
				{
					result.push_back( "#" );
				}
				
				result.push_back( token );
			}
			
			preceded_by_hash = false;
		}
	}
	
	static void splice_tokens( const token_list& tokens, token_list& result )
	{
		const size_t n_tokens = tokens.size();
		
		for ( int i = 0;  i < n_tokens;  ++i )
		{
			const plus::string& token = tokens[ i ];
			
			if ( token == "##" )
			{
				if ( result.empty() )
				{
					// complain
					return;
				}
				
				if ( ++i == n_tokens )
				{
					// complain
					return;
				}
				
				plus::var_string temp;
				
				temp.swap( result.back() );
				
				temp += tokens[ i ];
				
				temp.swap( result.back() );
			}
			else
			{
				result.push_back( token );
			}
		}
	}
	
	static bool expand_macros( const token_list&  input,
	                           bool               in_expression,
	                           bool               allow_calls,
	                           vxo::StrSet&       ignored,
	                           token_list&        output );
	
	static void expand_macro_call( const token_list&   pattern,
	                               const token_list&   replacement,
	                               const token_lists&  arg_list,
	                               bool                in_expression,
	                               vxo::StrSet&        ignored,
	                               token_list&         output )
	{
		token_list_map args = map_args( pattern, arg_list );
		
		token_list substituted;
		
		substitute_args( replacement, args, substituted );
		
		token_list spliced;
		
		splice_tokens( substituted, output );
	}
	
	static bool expand_macros( const token_list&  input,
	                           bool               in_expression,
	                           bool               allow_calls,
	                           vxo::StrSet&       ignored,
	                           token_list&        output )
	{
		const size_t n_tokens = input.size();
		
		for ( size_t i = 0;  i < n_tokens;  ++i )
		{
			const plus::string& token = input[ i ];
			
			if ( is_initial( token.front() ) )
			{
				const macro_t* macro = NULL;
				
				const bool _defined_ = in_expression  &&  token == "defined";
				const bool _option_  = !_defined_     &&  token == "__option";
				
				if ( !_defined_  &&  !_option_ )
				{
					plus::string predef_result = eval_predefined_macro( token );
					
					const bool preundefined = predef_result.empty();
					
					if ( predef_result != token )
					{
						output.push_back( preundefined ? token : predef_result );
						
						continue;
					}
					
					macro = find_macro( token );
				}
				
				if ( _defined_  ||  _option_  ||  (macro  &&  ! ignored.found( token )) )
				{
					if ( macro )
					{
						ignored.insert( token );
					}
					
					const bool needs_more_tokens = _option_  ||  _defined_  ||  (macro  &&  macro->pattern.size() > 1);
					
					if ( needs_more_tokens  &&  i + 1 == n_tokens )
					{
						return false;
					}
					
					const bool gets_paren = needs_more_tokens  &&  input[ i + 1 ] == "(";
					
					const bool call = _defined_ ? gets_paren : needs_more_tokens;
					
					if ( gets_paren  &&  allow_calls )
					{
						token_lists args;
						
						const bool got = get_macro_args( input, args, ++i );
						
						if ( !got )
						{
							return false;
						}
						
						if ( _defined_ )
						{
							add_boolean_token( is_defined( get_one_token( args ) ), output );
						}
						else if ( _option_ )
						{
							add_boolean_token( check_option( get_one_token( args ) ), output );
						}
						else
						{
							token_list spliced;
							
							expand_macro_call( macro->pattern,
							                   macro->replacement,
							                   args,
							                   in_expression,
							                   ignored,
							                   spliced );
							
							token_list semiexpanded;
							
							bool done;
							
							while ( true )
							{
								token_list local_output;
								
								done = expand_macros( spliced, in_expression, false, ignored, local_output );
								
								if ( done )
								{
									semiexpanded.swap( local_output );
									
									break;
								}
								
								if ( i >= n_tokens - 1 )
								{
									return false;
								}
								
								spliced.push_back( input[ ++i ] );
							}
							
							while ( true )
							{
								token_list local_output;
								
								done = expand_macros( semiexpanded, in_expression, true, ignored, local_output );
								
								if ( done )
								{
									output.insert( output.end(),
									               local_output.begin(),
									               local_output.end() );
									
									break;
								}
								
								if ( i >= n_tokens - 1 )
								{
									return false;
								}
								
								semiexpanded.push_back( input[ ++i ] );
							}
						}
						
						ignored.erase( token );
						
						continue;
					}
					else if ( !call  &&  _defined_ )
					{
						add_boolean_token( is_defined( input[ ++i ] ), output );
						
						continue;
					}
					else if ( !call  &&  macro )
					{
						const bool done = expand_macros( macro->replacement,
						                                 in_expression,
						                                 true,
						                                 ignored,
						                                 output );
						
						if ( !done )
						{
							return false;
						}
						
						ignored.erase( token );
						
						continue;
					}
					
					ignored.erase( token );
				}
			}
			
			output.push_back( token );
		}
		
		return true;
	}
	
	bool expand_macros( const token_list& input, token_list& output, bool in_expression )
	{
		vxo::StrSet ignored;
		
		return expand_macros( input, in_expression, true, ignored, output );
	}
	
}
