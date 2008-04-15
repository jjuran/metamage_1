// Pathnames.hh
// ------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_PATHNAMES_HH
#define POSEVEN_PATHNAMES_HH

// Standard C++
#include <string>

// POSIX
#include <unistd.h>

// Io
#include "io/files.hh"

// POSeven
#include "POSeven/Errno.hh"


namespace poseven
{
	
	inline void unlink( const char* pathname )
	{
		throw_posix_result( ::unlink( pathname ) );
	}
	
	inline void unlink( const std::string& pathname )
	{
		unlink( pathname.c_str() );
	}
	
	inline void rmdir( const char* pathname )
	{
		throw_posix_result( ::rmdir( pathname ) );
	}
	
	inline void rmdir( const std::string& pathname )
	{
		rmdir( pathname.c_str() );
	}
	
}

namespace io
{
	
	std::string get_filename( const std::string& pathname );
	
	inline std::string get_filename( const std::string& pathname, overload )
	{
		return get_filename( pathname );
	}
	
	inline std::string get_filename_string( const std::string& pathname, overload = overload() )
	{
		return get_filename( pathname );
	}
	
	inline std::string get_filename_string( const char* pathname, overload = overload() )
	{
		return get_filename( pathname );
	}
	
	std::string get_preceding_directory( const std::string& pathname );
	
	inline std::string get_preceding_directory( const std::string& pathname, overload )
	{
		return get_preceding_directory( pathname );
	}
	
	inline std::string get_parent_directory_of_directory( const std::string& pathname, overload = overload() )
	{
		std::string result = pathname;
		
		if ( *result.rbegin() != '/' )
		{
			result += '/';
		}
		
		result += "..";
		
		return result;
	}
	
	// Path descent
	
	inline std::string path_descent( std::string path, const char* name, std::size_t length )
	{
		if ( *path.rbegin() != '/' )
		{
			path += '/';
		}
		
		path.append( name, length );
		
		return path;
	}
	
	namespace path_descent_operators
	{
		
		inline std::string operator/( const std::string& path, const std::string& name )
		{
			return path_descent( path, name.data(), name.size() );
		}
		
		inline std::string operator/( const std::string& path, const char* name )
		{
			return path_descent( path, name, std::strlen( name ) );
		}
		
		inline std::string operator/( const char* path, const std::string& name )
		{
			return operator/( std::string( path ), name );
		}
		
	}
	
	// Delete
	
	inline void delete_file( const std::string& file, overload = overload() )
	{
		poseven::unlink( file );
	}
	
	inline void delete_file_only( const std::string& file, overload = overload() )
	{
		delete_file( file );
	}
	
	inline void delete_empty_directory( const std::string& dir, overload = overload() )
	{
		poseven::rmdir( dir );
	}
	
	inline void delete_empty_directory_only( const std::string& dir, overload = overload() )
	{
		delete_empty_directory( dir );
	}
	
}

#endif

