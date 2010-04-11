/*
	pfiles/common.hh
	----------------
	
	Joshua Juran
*/


#ifndef POSIXFILES_COMMON_HH
#define POSIXFILES_COMMON_HH

// Standard C++
#include <string>

// Standard C/C++
#include <cstring>

// Io
#include "io/io.hh"
#include "io/files.hh"
#include "io/walk.hh"

// poseven
#include "poseven/functions/fstat.hh"
#include "poseven/functions/lseek.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/rmdir.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/unlink.hh"
#include "poseven/functions/write.hh"
#include "poseven/sequences/directory_contents.hh"


namespace poseven
{
	
	struct posix_io_details
	{
		typedef std::string file_spec;
		typedef std::string optimized_directory_spec;
		
		typedef const std::string& filename_parameter;
		typedef       std::string  filename_result;
		
		typedef struct ::stat file_catalog_record;
		
		typedef fd_t stream;
		
		typedef std::size_t byte_count;
		
		typedef off_t position_offset;
	};
	
}

namespace io
{
	
	template <> struct filespec_traits< std::string   > : public poseven::posix_io_details {};
	template <> struct iostream_traits< poseven::fd_t > : public poseven::posix_io_details {};
	
	template <> struct directory_contents_traits< std::string >
	{
		typedef poseven::directory_contents_container container_type;
	};
	
	// Get file info
	
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
	
	inline std::string path_descent( const std::string& path, const char* name )
	{
		return path_descent( path, name, std::strlen( name ) );
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
	
	// Existence
	
	inline bool item_exists( const char* item, overload = overload() )
	{
		struct ::stat sb;
		
		return poseven::stat( item, sb );
	}
	
	inline bool item_exists( const std::string& item, overload = overload() )
	{
		return item_exists( item.c_str() );
	}
	
	
	inline bool item_is_file( const struct ::stat& sb, overload = overload() )
	{
		return S_ISREG( sb.st_mode );
	}
	
	inline bool item_is_directory( const struct ::stat& sb, overload = overload() )
	{
		return S_ISDIR( sb.st_mode );
	}
	
	
	inline bool file_exists( const char* item, overload = overload() )
	{
		struct ::stat sb;
		
		return poseven::stat( item, sb ) && item_is_file( sb );
	}
	
	inline bool file_exists( const std::string& item, overload = overload() )
	{
		return file_exists( item.c_str() );
	}
	
	
	inline bool directory_exists( const char* item, overload = overload() )
	{
		struct ::stat sb;
		
		return poseven::stat( item, sb ) && item_is_directory( sb );
	}
	
	inline bool directory_exists( const std::string& item, overload = overload() )
	{
		return directory_exists( item.c_str() );
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
	
	// Open
	
	inline nucleus::owned< poseven::fd_t > open_for_reading( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_rdonly );
	}
	
	inline nucleus::owned< poseven::fd_t > open_for_reading( const std::string& pathname, overload = overload() )
	{
		return open_for_reading( pathname.c_str(), overload() );
	}
	
	
	inline nucleus::owned< poseven::fd_t > open_for_writing( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_wronly );
	}
	
	inline nucleus::owned< poseven::fd_t > open_for_writing( const std::string& pathname, overload = overload() )
	{
		return open_for_writing( pathname.c_str(), overload() );
	}
	
	
	inline nucleus::owned< poseven::fd_t > open_for_io( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_rdwr );
	}
	
	inline nucleus::owned< poseven::fd_t > open_for_io( const std::string& pathname, overload = overload() )
	{
		return open_for_io( pathname.c_str(), overload() );
	}
	
	
	inline nucleus::owned< poseven::fd_t > open_truncated( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_wronly | poseven::o_trunc );
	}
	
	inline nucleus::owned< poseven::fd_t > open_truncated( const std::string& pathname, overload = overload() )
	{
		return open_truncated( pathname.c_str(), overload() );
	}
	
	// Stream operations
	
	inline std::size_t get_file_size( poseven::fd_t stream, overload = overload() )
	{
		struct stat status = poseven::fstat( stream );
		
		if ( !S_ISREG( status.st_mode ) )
		{
			poseven::throw_errno( ESPIPE );
		}
		
		return status.st_size;
	}
	
	template < class ByteCount >
	inline ssize_t read( poseven::fd_t fd, char* buffer, ByteCount byteCount, overload = overload() )
	{
		return poseven::read( fd, buffer, byteCount );
	}
	
	template < class ByteCount >
	inline ssize_t write( poseven::fd_t fd, const char* buffer, ByteCount byteCount, overload = overload() )
	{
		return poseven::write( fd, buffer, byteCount );
	}
	
	// Directory contents
	
	inline poseven::directory_contents_container directory_contents( const std::string& dir, overload = overload() )
	{
		return poseven::directory_contents( dir.c_str() );
	}
	
}

#endif

