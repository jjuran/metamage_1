/*
	pfiles/common.cc
	----------------
	
	Joshua Juran
*/

#include "pfiles/common.hh"

// plus
#include "plus/var_string.hh"


namespace io
{
	
	plus::string get_filename( const plus::string& pathname )
	{
		plus::var_string result = pathname;
		
		std::size_t last_slash = result.find_last_of( "/" );
		
		if ( last_slash == result.npos )
		{
			return result;
		}
		
		while ( last_slash == result.size() - 1 )
		{
			result.resize( last_slash );
			
			last_slash = result.find_last_of( "/" );
		}
		
		return result.substr( last_slash + 1, result.npos );
	}
	
	plus::string get_preceding_directory( const plus::string& pathname )
	{
		plus::var_string result = pathname;
		
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
	
	plus::string get_parent_directory_of_directory( const plus::string& pathname, overload )
	{
		plus::var_string result = pathname;
		
		if ( *(result.end() - 1) != '/' )
		{
			result += '/';
		}
		
		result += "..";
		
		return result;
	}
	
	plus::string path_descent( plus::var_string path, const char* name, std::size_t length )
	{
		if ( *(path.end() - 1) != '/' )
		{
			path += '/';
		}
		
		path.append( name, length );
		
		return path;
	}
	
	std::size_t get_file_size( poseven::fd_t stream, overload )
	{
		struct stat status = poseven::fstat( stream );
		
		if ( !S_ISREG( status.st_mode ) )
		{
			poseven::throw_errno( ESPIPE );
		}
		
		return status.st_size;
	}
	
}

