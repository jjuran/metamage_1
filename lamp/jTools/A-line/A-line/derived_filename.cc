/*
	derived_filename.cc
	-------------------
*/

#include "A-line/derived_filename.hh"

// Standard C++
#include <algorithm>

// Iota
#include "iota/strings.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"


namespace tool
{
	
	plus::string derived_filename( const plus::string& source_path )
	{
		const char* source_data = source_path.data();
		
		const std::size_t source_size = source_path.size();
		
		// The sentinel is a double slash inserted in the full pathname
		// to mark the project directory -- the portion following the
		// mark is the project-relative path.
		const std::size_t sentinel = source_path.find( "//" );
		
		ASSERT( ~sentinel );
		
		// Offset of project-relative path within the source pathname.
		const std::size_t path_offset = sentinel + STRLEN( "//" );
		
		// Length of project-relative path:
		const std::size_t subpath_length = source_size - path_offset;
		
		plus::var_string result;
		
		// Allocate space for the truncated filename
		result.resize( subpath_length );
		
		// Copy the project-relative path, minus extension, replacing
		// path separators with ':'
		std::replace_copy( &source_data[ path_offset ],
		                   &source_data[ source_size ],
		                   &result[ 0 ],
		                   '/',
		                   ':' );
		
		return result;
	}
	
	plus::string derived_pathname( const plus::string&  dir,
	                               const plus::string&  source_path,
	                               const char*          extension )
	{
		ASSERT( !dir.empty() );
		
		plus::var_string result;
		
		result += dir;
		
		if ( dir.back() != '/' )
		{
			result += '/';
		}
		
		result += derived_filename( source_path );
		result += extension;
		
		return result;
	}
}

