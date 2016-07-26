/*
	posixfs.cc
	----------
*/

#include "posixfs.hh"

// POSIX
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>
#include <string.h>

// plus
#include "plus/posix/dirname.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/types.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	static
	void path_error( const char* path, int err = errno )
	{
		Value error( "errno", Op_mapping, Integer ( err ) );
		Value desc ( "desc",  Op_mapping, strerror( err ) );
		Value path_( "path",  Op_mapping, path );
		
		Value exception( error, Value( desc, path_ ) );
		
		throw user_exception( exception, source_spec() );
	}
	
	static
	void path_error( const char* path, const char* msg )
	{
		Value desc ( "desc",  Op_mapping, msg  );
		Value path_( "path",  Op_mapping, path );
		
		Value exception( desc, path_ );
		
		throw user_exception( exception, source_spec() );
	}
	
	static
	Value v_dirname( const Value& v )
	{
		return plus::dirname( v.string() );
	}
	
	static
	Value v_listdir( const Value& v )
	{
		const char* path = v.string().c_str();
		
		DIR* dir = opendir( path );
		
		if ( dir == NULL )
		{
			path_error( path );
		}
		
		list_builder list;
		
		while ( const dirent* entry = readdir( dir ) )
		{
			const char* name = entry->d_name;
			
			if ( name[ 0 ] == '.'  &&  name[ 1 + (name[ 1 ] == '.') ] == '\0' )
			{
				// "." or ".."
				continue;
			}
			
			list.append( name );
		}
		
		closedir( dir );
		
		return make_array( list );
	}
	
	static
	Value v_load( const Value& v )
	{
		const char* path = v.string().c_str();
		
		/*
			Open nonblocking in case it's a FIFO.
			Nonblocking I/O shouldn't affect reading from a regular file.
		*/
		
		int fd = open( path, O_RDONLY | O_NONBLOCK );
		
		if ( fd < 0 )
		{
			path_error( path );
		}
		
		struct stat st;
		
		int nok = fstat( fd, &st );
		
		if ( nok )
		{
			int saved_errno = errno;
			
			close( fd );
			
			path_error( path, saved_errno );
		}
		
		if ( ! S_ISREG( st.st_mode ) )
		{
			close( fd );
			
			path_error( path, "not a regular file" );
		}
		
		const off_t len = st.st_size;
		
		plus::string result;
		
		char* p = result.reset( len );
		
		ssize_t n_read = read( fd, p, len );
		
		if ( n_read < 0 )
		{
			int saved_errno = errno;
			
			close( fd );
			
			path_error( path, saved_errno );
		}
		
		close( fd );
		
		if ( n_read != len )
		{
			path_error( path, "unexpected short read" );
		}
		
		return result;
	}
	
	static const Value c_str = c_str_vtype;
	
	const proc_info proc_dirname = { "dirname", &v_dirname, &c_str };
	const proc_info proc_listdir = { "listdir", &v_listdir, &c_str };
	const proc_info proc_load    = { "load",    &v_load,    &c_str };
	
}
