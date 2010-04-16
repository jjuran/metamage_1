/*
	pfiles/common.cc
	----------------
	
	Joshua Juran
*/

#include "pfiles/common.hh"

// more-libc
#include "more/string.h"

// gear
#include "gear/find.hh"


namespace io
{
	
	plus::string get_preceding_directory( const plus::string& pathname )
	{
		const char* begin = pathname.data();
		
		const std::size_t length = pathname.size();
		
		if ( length != 0 )
		{
			const char* end = begin + length;
			
			if ( end[ -1 ] == '/' )
			{
				--end;
			}
			
			if ( const char* last_slash = gear::find_last_match( begin, end, '/' ) )
			{
				// substring constructor, may share the buffer
				return plus::string( pathname, 0, last_slash + 1 - begin );
			}
			
		}
		
		return ".";
	}
	
	plus::string path_descent( const plus::string& path, const char* name, std::size_t length )
	{
		const bool has_trailing_slash = path.back() == '/';
		
		const std::size_t path_size = path.size();
		
		const std::size_t size = path_size + !has_trailing_slash + length;
		
		plus::string result;
		
		char* p = result.reset( size );
		
		p = (char*) mempcpy( p, path.data(), path_size );
		
		if ( !has_trailing_slash )
		{
			*p++ = '/';
		}
		
		p = (char*) mempcpy( p, name, length );
		
		return result;
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

