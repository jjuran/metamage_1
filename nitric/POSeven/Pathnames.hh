// ============
// Pathnames.hh
// ============

#ifndef POSEVEN_PATHNAMES_HH
#define POSEVEN_PATHNAMES_HH

// Standard C++
#include <string>

// Io
#include "io/files.hh"


namespace POSeven
{
	
}

namespace io
{
	
	inline std::string get_filename( const std::string& pathname )
	{
		std::string result = pathname;
		
		std::size_t last_slash = result.find_last_of( "/" );
		
		if ( last_slash == result.npos )
		{
			return result;
		}
		
		if ( last_slash == result.size() - 1 )
		{
			result.resize( last_slash );
			
			last_slash = result.find_last_of( "/" );
		}
		
		return result.substr( last_slash + 1, result.npos );
	}
	
	inline std::string get_preceding_directory( const std::string& pathname )
	{
		std::string result = pathname;
		
		std::size_t last_slash = result.find_last_of( "/" );
		
		if ( last_slash == result.npos )
		{
			return ".";
		}
		
		if ( last_slash == result.size() - 1 )
		{
			result.resize( last_slash );
			
			last_slash = result.find_last_of( "/" );
		}
		
		result.resize( last_slash + 1 );
		
		return result;
	}
	
	inline std::string get_parent_directory_of_directory( const std::string& pathname )
	{
		std::string result = pathname;
		
		if ( result.back() != '/' )
		{
			result += '/';
		}
		
		result += "..";
		
		return result;
	}
	
}

#endif

