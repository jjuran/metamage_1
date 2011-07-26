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
	
	plus::string path_descent( plus::var_string path, const char* name, std::size_t length )
	{
		const bool has_trailing_slash = path.back() == '/';
		
		if ( !has_trailing_slash )
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

