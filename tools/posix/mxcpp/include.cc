/*
	include.cc
	----------
*/

#include "include.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// Extended API Set Part 2
#include "extended-api-set/part-2.h"

// iota
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// vxo
#include "vxo/strmap.hh"
#include "vxo/strset.hh"

// poseven
#include "poseven/functions/fstatat.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/write.hh"

// mxcpp
#include "config.hh"
#include "exception.hh"
#include "macro.hh"
#include "pragma_once.hh"
#include "preprocess.hh"


using vxo::StrSet;

typedef vxo::StrMap_to< vxo::String > StrMap;

namespace tool
{
	
	namespace p7 = poseven;
	
	
	std::vector< const char* > global_include_search_paths;
	
	static std::vector< p7::fd_t > global_include_search_dirs;
	
	static StrSet global_paths_once_included;
	static StrMap global_memoized_include_guards;
	
	
	static p7::fd_t open_dir( const char* path )
	{
		return p7::open( path, p7::o_rdonly | p7::o_directory ).release();
	}
	
	static size_t open_dirs()
	{
		const size_t n = global_include_search_paths.size();
		
		global_include_search_dirs.reserve( n );
		
		typedef std::vector< const char* >::const_iterator Iter;
		
		#define paths global_include_search_paths
		
		for ( Iter it = paths.begin();  it != paths.end();  ++it )
		{
			global_include_search_dirs.push_back( open_dir( *it ) );
		}
		
		#undef paths
		
		return n;
	}
	
	void mark_current_source_once_included()
	{
		const plus::string& path = get_current_source_path();
		
		global_paths_once_included.insert( path );
	}
	
	void memoize_include_guard( const plus::string& file, const plus::string& guard )
	{
		global_memoized_include_guards[ file ] = guard;
	}
	
	static bool should_skip_include( const plus::string& path )
	{
		if ( global_paths_once_included.found( path ) )
		{
			return true;
		}
		
		StrMap::const_iterator it = global_memoized_include_guards.found( path );
		
		return it  &&  is_defined( it->second );
	}
	
	static size_t lookup_path( const plus::string& path )
	{
		for ( size_t i = 0;  i < global_include_search_dirs.size();  ++i )
		{
			const p7::fd_t dirfd = global_include_search_dirs[ i ];
			
			struct stat sb;
			
			const bool exists = dirfd >= 0  &&  p7::fstatat( dirfd, path, sb );
			
			if ( exists )
			{
				return i;
			}
		}
		
		throw exception( path.c_str() );
	}
	
	void include_file( const plus::string& target )
	{
		const char* quote = target.c_str();
		
		const char* begin = quote + 1;
		
		const char end_char = *quote == '<' ? '>'
		                    : *quote == '"' ? '"'
		                    :                 '\0';
		
		plus::string include_path;
		
		if ( end_char != '\0' )
		{
			const char* end = strchr( begin, end_char );
			
			if ( end == NULL )
			{
				throw unterminated_include_path();
			}
			
			include_path.assign( begin, end );
		}
		else
		{
			if ( const macro_t* macro = find_macro( target ) )
			{
				if ( macro->pattern().size() > 1 )
				{
					throw exception( "#include MACRO where MACRO takes arguments" );
				}
				
				const token_list& replacement = macro->replacement();
				
				const size_t n_tokens = replacement.size();
				
				if ( n_tokens == 1 )
				{
					const plus::string& actual_target = replacement[0];
					
					if ( actual_target[0] != '"' )
					{
						throw exception( "#include MACRO where MACRO is not a string literal" );
					}
					
					include_path.assign( &actual_target[1], actual_target.size() - 2 );
				}
				else if ( n_tokens >= 3 )
				{
					if ( replacement[0] != "<"  ||  replacement[ n_tokens - 1 ] != ">" )
					{
						throw exception( "#include MACRO where MACRO is not within angle brackets" );
					}
					
					plus::var_string actual_path = replacement[1];
					
					for ( int i = 2;  i < n_tokens - 1;  ++i )
					{
						actual_path += replacement[ i ];
					}
					
					include_path.swap( actual_path );
				}
				else
				{
					throw exception( "#include MACRO where MACRO is two tokens." );
				}
				
			}
			else
			{
				throw exception( "Include is not \"\" or <>, or a macro" );
			}
		}
		
		if ( should_skip_include( include_path ) )
		{
			return;
		}
		else if ( including_once() )
		{
			global_paths_once_included.insert( include_path );
		}
		
		static const size_t n_search_dirs = open_dirs();
		
		const size_t i = lookup_path( include_path );
		
		const p7::fd_t dirfd = global_include_search_dirs[ i ];
		
		const char* found_path = global_include_search_paths[ i ];
		
		
		if ( global_config_debug )
		{
			p7::write( p7::stderr_fileno, include_path );
			
			p7::write( p7::stderr_fileno, STR_LEN( "\n" ) );
		}
		
		preprocess_file( found_path, dirfd, include_path );
	}
	
}
