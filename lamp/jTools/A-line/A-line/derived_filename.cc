/*
	derived_filename.cc
	-------------------
*/

#include "A-line/derived_filename.hh"

// Standard C
#include <string.h>

// Standard C++
#include <algorithm>

// more-libc
#include "more/string.h"

// Iota
#include "iota/strings.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"


namespace tool
{
	
	static
	plus::string derived_filename( const plus::string& source_path )
	{
		const char* source_data = source_path.data();
		
		const size_t source_size = source_path.size();
		
		// Offset of project-relative path within the source pathname.
		size_t path_offset = 0;
		
		// The sentinel is a double slash inserted in the full pathname
		// to mark the project directory -- the portion following the
		// mark is the project-relative path.
		const size_t sentinel = source_path.find( "//" );
		
		if ( ~sentinel )
		{
			path_offset = sentinel + STRLEN( "//" );
		}
		else
		{
			const size_t last_slash = source_path.find_last_of( "/" );
			
			if ( ~last_slash )
			{
				path_offset = last_slash + STRLEN( "/" );
			}
		}
		
		// Length of project-relative path:
		const size_t subpath_length = source_size - path_offset;
		
		plus::string result;
		
		// Allocate space for the truncated filename
		char* p = result.reset( subpath_length );
		
		// Copy the project-relative path, minus extension, replacing
		// path separators with ':'
		std::replace_copy( &source_data[ path_offset ],
		                   &source_data[ source_size ],
		                   p,
		                   '/',
		                   ':' );
		
		return result;
	}
	
	plus::string derived_pathname( const plus::string&  dir,
	                               const plus::string&  source_path,
	                               const char*          extension )
	{
		ASSERT( !dir.empty() );
		
		plus::string filename = derived_filename( source_path );
		
		const bool has_trailing_slash = dir.back() == '/';
		
		const size_t dir_size = dir.size();
		const size_t src_size = filename.size();
		const size_t ext_size = strlen( extension );
		
		const size_t size = dir_size + !has_trailing_slash + src_size + ext_size;
		
		plus::string result;
		
		char* p = result.reset( size );
		
		p = (char*) mempcpy( p, dir.data(), dir_size );
		
		if ( !has_trailing_slash )
		{
			*p++ = '/';
		}
		
		p = (char*) mempcpy( p, filename.data(), src_size );
		
		p = (char*) mempcpy( p, extension, ext_size );
		
		return result;
	}
}
