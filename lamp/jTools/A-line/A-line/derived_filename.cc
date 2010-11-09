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
		// The last dot in the source path.
		// There should definitely be a dot in the filename, or the file
		// wouldn't be compiled in the first place.
		const std::size_t dot = source_path.find_last_of( '.' );
		
		// The sentinel is a double slash inserted in the full pathname
		// to mark the project directory -- the portion following the
		// mark is the project-relative path.
		const std::size_t sentinel = source_path.find( "//" );
		
		ASSERT( sentinel < dot );
		
		// Offset of project-relative path within the source pathname.
		const std::size_t path_offset = sentinel + STRLEN( "//" );
		
		// Length of resulting filename before extension is appended:
		const std::size_t truncated_length = dot - path_offset;
		
		plus::var_string result;
		
		// Allocate space for the truncated filename
		result.resize( truncated_length );
		
		// Copy the project-relative path, minus extension, replacing
		// path separators with ':'
		std::replace_copy( &source_path[ path_offset ],
		                   &source_path[ dot         ],
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

