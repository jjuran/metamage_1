/*
	pfiles/common.cc
	----------------
	
	Joshua Juran
*/

#include "pfiles/common.hh"


namespace io
{
	
	std::string get_filename( const std::string& pathname )
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
	
	std::string get_preceding_directory( const std::string& pathname )
	{
		std::string result = pathname;
		
		std::size_t last_slash = result.find_last_of( "/" );
		
		if ( last_slash == result.size() - 1 )
		{
			result.resize( last_slash );
			
			last_slash = result.find_last_of( "/" );
		}
		
		if ( last_slash == result.npos )
		{
			return ".";
		}
		
		result.resize( last_slash + 1 );
		
		return result;
	}
	
}

