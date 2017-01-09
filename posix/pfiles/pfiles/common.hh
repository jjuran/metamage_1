/*
	pfiles/common.hh
	----------------
	
	Joshua Juran
*/


#ifndef POSIXFILES_COMMON_HH
#define POSIXFILES_COMMON_HH

// Standard C/C++
#include <cstring>

// iota
#include "iota/string_traits.hh"

// plus
#include "plus/var_string.hh"

// Io
#ifndef IO_IO_HH
#include "io/io.hh"
#endif
#ifndef IO_FILES_HH
#include "io/files.hh"
#endif
#ifndef IO_WALKFWD_HH
#include "io/walk_fwd.hh"
#endif

// poseven
#ifndef POSEVEN_FUNCTIONS_FSTAT_HH
#include "poseven/functions/fstat.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_LSEEK_HH
#include "poseven/functions/lseek.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_LSTAT_HH
#include "poseven/functions/lstat.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_OPEN_HH
#include "poseven/functions/open.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_READ_HH
#include "poseven/functions/read.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_RMDIR_HH
#include "poseven/functions/rmdir.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_UNLINK_HH
#include "poseven/functions/unlink.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_WRITE_HH
#include "poseven/functions/write.hh"
#endif
#ifndef POSEVEN_SEQUENCES_DIRECTORYCONTENTS_HH
#include "poseven/sequences/directory_contents.hh"
#endif


namespace poseven
{
	
	struct posix_io_details
	{
		typedef plus::string file_spec;
		typedef plus::string optimized_directory_spec;
		
		typedef const plus::string& filename_parameter;
		typedef       plus::string  filename_result;
		
		typedef struct stat file_catalog_record;
		
		typedef fd_t stream;
		
		typedef std::size_t byte_count;
		
		typedef off_t position_offset;
	};
	
}

namespace io
{
	
	template <> struct filespec_traits< plus::string  > : public poseven::posix_io_details {};
	template <> struct iostream_traits< poseven::fd_t > : public poseven::posix_io_details {};
	
	template <> struct directory_contents_traits< plus::string >
	{
		typedef poseven::directory_contents_container container_type;
	};
	
	// Get file info
	
	plus::string get_preceding_directory( const plus::string& pathname );
	
	inline plus::string get_preceding_directory( const plus::string& pathname, overload )
	{
		return get_preceding_directory( pathname );
	}
	
	// Path descent
	
	plus::string path_descent( const plus::string& path, const char* name, std::size_t length );
	
	inline plus::string path_descent( const plus::string& path, const char* name )
	{
		return path_descent( path, name, std::strlen( name ) );
	}
	
	namespace path_descent_operators
	{
		
		template < class String >
		plus::string operator/( const plus::string& path, const String& name )
		{
			return path_descent( path,
			                     iota::get_string_data( name ),
			                     iota::get_string_size( name ) );
		}
		
	}
	
	// Existence
	
	inline bool item_exists( const char* item, overload = overload() )
	{
		struct stat sb;
		
		return poseven::lstat( item, sb );
	}
	
	inline bool item_exists( const plus::string& item, overload = overload() )
	{
		return item_exists( item.c_str() );
	}
	
	
	inline bool item_is_file( const struct stat& sb, overload = overload() )
	{
		return S_ISREG( sb.st_mode );
	}
	
	inline bool item_is_directory( const struct stat& sb, overload = overload() )
	{
		return S_ISDIR( sb.st_mode );
	}
	
	
	inline bool file_exists( const char* item, overload = overload() )
	{
		struct stat sb;
		
		return poseven::lstat( item, sb ) && !item_is_directory( sb );
	}
	
	inline bool file_exists( const plus::string& item, overload = overload() )
	{
		return file_exists( item.c_str() );
	}
	
	
	inline bool directory_exists( const char* item, overload = overload() )
	{
		struct stat sb;
		
		return poseven::lstat( item, sb ) && item_is_directory( sb );
	}
	
	inline bool directory_exists( const plus::string& item, overload = overload() )
	{
		return directory_exists( item.c_str() );
	}
	
	// Delete
	
	inline void delete_file( const char* file, overload = overload() )
	{
		poseven::unlink( file );
	}
	
	inline void delete_file_only( const char* file, overload = overload() )
	{
		delete_file( file );
	}
	
	inline void delete_empty_directory( const char* dir, overload = overload() )
	{
		poseven::rmdir( dir );
	}
	
	inline void delete_empty_directory_only( const char* dir, overload = overload() )
	{
		delete_empty_directory( dir );
	}
	
	
	inline void delete_file( const plus::string& file, overload = overload() )
	{
		delete_file( file.c_str() );
	}
	
	inline void delete_file_only( const plus::string& file, overload = overload() )
	{
		delete_file_only( file.c_str() );
	}
	
	inline void delete_empty_directory( const plus::string& dir, overload = overload() )
	{
		delete_empty_directory( dir.c_str() );
	}
	
	inline void delete_empty_directory_only( const plus::string& dir, overload = overload() )
	{
		delete_empty_directory_only( dir.c_str() );
	}
	
	// Open
	
	inline nucleus::owned< poseven::fd_t > open_for_reading( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_rdonly );
	}
	
	inline nucleus::owned< poseven::fd_t > open_for_reading( const plus::string& pathname, overload = overload() )
	{
		return open_for_reading( pathname.c_str(), overload() );
	}
	
	
	inline nucleus::owned< poseven::fd_t > open_for_writing( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_wronly );
	}
	
	inline nucleus::owned< poseven::fd_t > open_for_writing( const plus::string& pathname, overload = overload() )
	{
		return open_for_writing( pathname.c_str(), overload() );
	}
	
	
	inline nucleus::owned< poseven::fd_t > open_for_io( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_rdwr );
	}
	
	inline nucleus::owned< poseven::fd_t > open_for_io( const plus::string& pathname, overload = overload() )
	{
		return open_for_io( pathname.c_str(), overload() );
	}
	
	
	inline nucleus::owned< poseven::fd_t > open_truncated( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_wronly | poseven::o_trunc );
	}
	
	inline nucleus::owned< poseven::fd_t > open_truncated( const plus::string& pathname, overload = overload() )
	{
		return open_truncated( pathname.c_str(), overload() );
	}
	
	// Stream operations
	
	std::size_t get_file_size( poseven::fd_t stream, overload = overload() );
	
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
	
	inline poseven::directory_contents_container directory_contents( const plus::string& dir, overload = overload() )
	{
		return poseven::directory_contents( dir.c_str() );
	}
	
}

#endif
