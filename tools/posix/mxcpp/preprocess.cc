/*
	preprocess.cc
	-------------
*/

#include "preprocess.hh"

// Standard C++
#include <list>

// Standard C
#include <ctype.h>

// gear
#include "gear/find.hh"
#include "gear/parse_decimal.hh"

// debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"

// mxcpp
#include "conditional.hh"
#include "config.hh"
#include "exception.hh"
#include "file_state.hh"
#include "include.hh"
#include "line.hh"
#include "print.hh"
#include "tokenize.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	class unterminated_string {};
	
	
	static bool globally_needs_line_directive = true;
	
	enum include_guard_detection_state
	{
		detection_initial,
		detected_ifndef,
		detected_define,
		detected_absent
	};
	
	static file_state* global_file_state = NULL;
	
	static std::list< file_state > global_file_stack;
	
	
	const plus::string& get_current_source_path()
	{
		ASSERT( global_file_state != NULL );
		
		return global_file_state->get_path();
	}
	
	const plus::string& get_current_FILE()
	{
		ASSERT( global_file_state != NULL );
		
		return global_file_state->get_FILE();
	}
	
	unsigned get_current_LINE()
	{
		ASSERT( global_file_state != NULL );
		
		return global_file_state->get_LINE();
	}
	
	static plus::string unquote( const plus::string& quoted_path )
	{
		if ( quoted_path.size() > 2 )
		{
			return plus::string( &quoted_path[1], quoted_path.size() - 2 );
		}
		
		return plus::string();
	}
	
	void set_line( const plus::string& line, const plus::string& file )
	{
		ASSERT( global_file_state != NULL );
		
		const size_t nth_line = gear::parse_unsigned_decimal( line.c_str() );
		
		global_file_state->set_line( nth_line, unquote( file ) );
	}
	
	static const char* skip_string( const char* quote, const char* end )
	{
		const char c = *quote;
		
		const char* p = quote;
		
		while ( ++p < end )
		{
			if ( *p == '\\' )
			{
				++p;
			}
			else if ( *p == c )
			{
				return p;
			}
		}
		
		throw exception( "unterminated_string" );
	}
	
	static file_state& new_file( const char*          dir,
	                             const plus::string&  path,
	                             p7::fd_t             fd )
	{
		global_file_stack.push_back( file_state( dir, path, fd ) );
		
		return global_file_stack.back();
	}
	
	static const unsigned char* space = (const unsigned char*) "\4 \t\n\r";
	
	static bool is_blank_line( const plus::string& line )
	{
		return ! gear::find_first_nonmatch( line.data(), line.size(), space );
	}
	
	static void trim_trailing_whitespace( plus::var_string& line )
	{
		char* begin = &line[ 0 ];
		char* end   = begin + line.size();
		
		if ( begin == end )
		{
			return;
		}
		
		while ( end-- > begin  &&  isspace( *end ) )
		{
			continue;
		}
		
		*++end = global_newline_char;
		
		line.resize( end - begin + 1 );
	}
	
	static void append_to_string( plus::var_string& out, const char* in, size_t length, char tail )
	{
		out.reserve( out.size() + length + 1 );
		
		out.append( in, length );
		
		out += tail;
	}
	
	static plus::string get_next_processed_line( file_state& file )
	{
		plus::var_string result;
		
		bool in_block_comment = false;
		
	next_line:
		
		const plus::string* input_line = file.get_logical_line();
		
		if ( input_line == NULL )
		{
			// FIXME:  Check if we're in_block_comment
			
			return result;
		}
		
		const char* begin = input_line->c_str();
		
		const char* end = begin + input_line->size();
		
		const char* p = begin;
		
		while ( p < end )
		{
			if ( in_block_comment )
			{
				const char* end_of_comment = strstr( p, "*/" );
				
				in_block_comment = !end_of_comment;
				
				if ( in_block_comment )
				{
					++file.its_buffered_blank_line_count;
					
					goto next_line;
				}
				
				begin = p = end_of_comment + 2;
				
				continue;
			}
			
			if ( p[0] == '/' )
			{
				if ( p[1] == '/' )
				{
					// line comment
					
					append_to_string( result, begin, p - begin, global_newline_char );
					
					p = begin;
					
					break;
				}
				
				if ( p[1] == '*' )
				{
					in_block_comment = true;
					
					if ( p != begin )
					{
						append_to_string( result, begin, p - begin, ' ' );
					}
					
					continue;
				}
			}
			else if ( *p == '"'  ||  *p == '\'' )
			{
				p = skip_string( p, end );
			}
			
			++p;
		}
		
		result.append( begin, p );
		
		trim_trailing_whitespace( result );
		
		if ( result.empty() )
		{
			++file.its_buffered_blank_line_count;
			
			goto next_line;
		}
		
		return result;
	}
	
	struct include_guard_detection_data
	{
		include_guard_detection_state  state;
		plus::string                   guard_macro;
		size_t                         conditional_depth;
	};
	
	static void detect_include_guards( include_guard_detection_data& data, const plus::string& line )
	{
		token_list guard_tokens;
		
		switch ( data.state )
		{
			case detection_initial:
			case detected_ifndef:
				tokenize( line, guard_tokens );
				
				if ( guard_tokens.size() == 3  &&  guard_tokens[0] == "#" )
				{
					const bool want_ifndef = data.state != detected_ifndef;
					
					const char* directive = want_ifndef ? "ifndef" : "define";
					
					if ( guard_tokens[1] == directive )
					{
						if ( want_ifndef )
						{
							data.guard_macro = guard_tokens[2];
							
							data.state = detected_ifndef;
							break;
						}
						else if ( guard_tokens[2] == data.guard_macro )
						{
							data.state = detected_define;
							break;
						}
					}
				}
				
				data.state = detected_absent;
				break;
			
			case detected_define:
				if ( get_conditional_depth() == data.conditional_depth )
				{
					// Stuff follows the matching #endif
					data.state = detected_absent;
				}
				
				break;
			
			case detected_absent:
				// do nothing
			default:
				break;
		}
	}
	
	static void preprocess_file( const char*          dir_path,
	                             const plus::string&  path,
	                             p7::fd_t             fd )
	{
		if ( globally_needs_line_directive  && !global_file_stack.empty() )
		{
			print( global_file_stack.back().get_line_directive( true ) );
		}
		
		globally_needs_line_directive = true;
		
		// Construct the new file_state in a separate function to make sure the
		// stack gets popped before we recurse.
		file_state& file = new_file( dir_path, path, fd );
		
		global_file_state = &file;
		
		++global_include_depth;
		
		include_guard_detection_data guard_data;
		
		guard_data.state             = detection_initial;
		guard_data.conditional_depth = get_conditional_depth();
		
		plus::var_string logical_line;
		
		plus::string processed_line;
		
		while ( !(processed_line = get_next_processed_line( file )).empty() )
		{
			if ( is_blank_line( processed_line ) )
			{
				++file.its_buffered_blank_line_count;
			}
			else
			{
				detect_include_guards( guard_data, processed_line );
				
				logical_line += processed_line;
				
				const bool done = process_line( logical_line );
				
				if ( !done )
				{
					// A macro call is split across multiple lines.
					// Pretend this line is blank for bookkeeping purposes and
					// concatenate the content with the next line for reprocessing.
					
					++file.its_buffered_blank_line_count;
					
					continue;
				}
				
				if ( global_file_state == NULL )
				{
					global_file_state = &file;
					
					globally_needs_line_directive = true;
				}
				
				trim_trailing_whitespace( logical_line );
				
				if ( is_blank_line( logical_line ) )
				{
					++file.its_buffered_blank_line_count;
				}
				else
				{
					if ( globally_needs_line_directive  ||  file.its_buffered_blank_line_count > 9 )
					{
						print( file.get_line_directive( globally_needs_line_directive ) );
						
						globally_needs_line_directive = false;
					}
					else
					{
						const char* nine_lines = global_newline_char == '\n' ? "\n\n\n\n\n\n\n\n\n"
						                                                     : "\r\r\r\r\r\r\r\r\r";
						
						print( nine_lines, file.its_buffered_blank_line_count );
					}
					
					print( logical_line );
					
					file.its_buffered_blank_line_count = 0;
				}
			}
			
			logical_line.reset();
		}
		
		if ( globally_needs_line_directive )
		{
			print( file.get_line_directive( true ) );
		}
		
		if ( guard_data.state == detected_define )
		{
			memoize_include_guard( file.get_path(), guard_data.guard_macro );
		}
		
		--global_include_depth;
		
		global_file_stack.pop_back();
		
		global_file_state = NULL;
	}
	
	void preprocess_file( const char *dir_path, int dirfd, const plus::string& path )
	{
		preprocess_file( dir_path,
		                 path,
		                 p7::openat( p7::fd_t( dirfd ), path, p7::o_rdonly ) );
	}
	
	void preprocess_file( const char *path )
	{
		preprocess_file( ".", path, p7::open( path, p7::o_rdonly ) );
	}
	
}
