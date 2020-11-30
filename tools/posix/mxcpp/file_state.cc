/*
	file_state.cc
	-------------
*/

#include "file_state.hh"

// Standard C
#include <ctype.h>

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// text-input
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/types/errno_t.hh"

// mxcpp
#include "config.hh"
#include "include.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	int global_include_depth = -1;
	
	
	static void convert_unix_path_to_mac( plus::string& path )
	{
	#ifdef __RELIX__
		
		char buffer[ 1024 ];  // should be more than enough for a full pathname
		
		ssize_t size = _realpathat( AT_FDCWD, path.c_str(), buffer, sizeof buffer, REALPATH_OUTPUT_HFS );
		
		if ( size >= 0 )
		{
			path.assign( buffer, size );
		}
		else
		{
			p7::throw_errno( errno );
		}
		
	#else
		
		p7::throw_errno( ENOSYS );
		
	#endif
	}
	
	
	const plus::string* file_state::get_next_line()
	{
		++its_nth_line;
		
		return get_line_bare_from_feed( its_feed, its_reader );
	}
	
	const plus::string* file_state::get_logical_line()
	{
		plus::var_string logical_line;
		
		while ( const plus::string* s = get_next_line() )
		{
			logical_line.append( *s );
			
			logical_line += global_newline_char;
			
			if ( s->size() < 2 )
			{
				break;
			}
			
			const char* begin = s->c_str();
			
			const char* end = begin + s->size();
			
			const char* q = end;
			
			while ( q > begin  &&  isspace( *--q ) )
			{
				continue;
			}
			
			if ( *q != '\\' )
			{
				// Not a continuation
				break;
			}
			
			++its_buffered_blank_line_count;
			
			const size_t extra = end - q;
			
			logical_line.resize( logical_line.size() - extra - 1 );
		}
		
		if ( logical_line.empty() )
		{
			return NULL;
		}
		
		its_logical_line = logical_line.move();
		
		return &its_logical_line;
	}
	
	void file_state::set_line( size_t nth_line, const plus::string& path )
	{
		// This sets the line number of the *next* line.  Since reading the next
		// line will increment its_nth_line, we need to decrement it here to
		// cancel it out.
		
		its_nth_line = nth_line - 1;
		
		if ( !path.empty() )
		{
			its_line_directed_file = path;
		}
	}
	
	plus::string file_state::get_line_directive( bool with_file ) const
	{
		if ( global_line_style == no_line_directives )
		{
			return "";
		}
		
		plus::var_string line_directive = "#line ";
		
		line_directive += gear::inscribe_unsigned_decimal( its_nth_line );
		
		if ( with_file )
		{
			line_directive += "  \"";
			
			plus::var_string path;
			
			if ( !its_line_directed_file.empty() )
			{
				path = its_line_directed_file;
			}
			else
			{
				if ( its_path[0] != '/' )
				{
					path = its_dir_path;
					
					if ( *(path.end() - 1) != '/' )
					{
						path += '/';
					}
				}
				
				path += its_path;
				
				if ( global_line_style == mac_line_directives )
				{
					convert_unix_path_to_mac( path );
				}
			}
			
			line_directive += path;
			
			line_directive += "\"  /* stack depth ";
			line_directive += gear::inscribe_unsigned_decimal( global_include_depth );
			line_directive += " */";
		}
		
		line_directive += global_newline_char;
		
		return line_directive.move();
	}
	
}
