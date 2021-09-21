/*
	posixfs.cc
	----------
*/

#include "varyx/posix/posixfs.hh"

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// POSIX
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <utime.h>
#include <sys/socket.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>  // for remove()
#include <string.h>

// plus
#include "plus/posix/dirname.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/string-utils.hh"
#include "vlib/types.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/table.hh"
#include "vlib/types/type.hh"
#include "vlib/types/unitary.hh"

// varyx-posix
#include "varyx/posix/exception.hh"
#include "varyx/posix/file_descriptor.hh"


namespace varyx
{
namespace posix
{
	
	using vlib::Boolean;
	
	using namespace vlib;
	
	
	static const Type string = string_vtype;
	
	static
	int fd_cast( const Value& v )
	{
		if ( const FileDescriptor* fd = v.is< FileDescriptor >() )
		{
			return fd->get();
		}
		
		return integer_cast< int >( v );
	}
	
	static const char* file_types[] =
	{
		"(0)",
		"FIFO",
		"CHR",
		"(3)",
		"DIR",
		"(5)",
		"BLK",
		"(7)",
		"REG",
		"(9)",
		"LNK",
		"(11)",
		"SOCK",
		"(13)",
		"(14)",
		"(15)",
	};
	
	static
	Value make_stat( const struct stat& st )
	{
		list_builder result;
		
		const char* type = file_types[ (st.st_mode & S_IFMT) >> 12 ];
		
		result.append( mapping( "type", String( type ) ) );
		
		#define APPEND( field )  \
		result.append( mapping( #field, Integer( st.st_##field ) ) )
		
		APPEND( dev   );
		APPEND( ino   );
		APPEND( mode  );
		APPEND( nlink );
		APPEND( uid   );
		APPEND( gid   );
		APPEND( rdev  );
		APPEND( atime );
		APPEND( mtime );
		APPEND( ctime );
		
		APPEND( size    );
		APPEND( blocks  );
		APPEND( blksize );
		
		#undef APPEND
		
		return Table( string, make_array( result ) );
	}
	
	
	static
	Value v_append( const Value& v )
	{
		const char*          path = first( v ).string().c_str();
		const plus::string&  data = rest ( v ).string();
		
		int fd = open( path, O_WRONLY | O_APPEND, 0 );
		
		if ( fd < 0 )
		{
			fd_error( fd );
		}
		
		const size_t size = data.size();
		
		ssize_t n_written = write( fd, data.data(), size );
		
		if ( n_written < size )
		{
			int saved_errno = errno;
			
			close( fd );
			
			path_error( path, n_written < 0 ? saved_errno : EINTR );
		}
		
		close( fd );
		
		return Value_nothing;
	}
	
	static
	Value v_chdir( const Value& v )
	{
		const char* path = v.string().c_str();
		
		int nok = chdir( path );
		
		if ( nok )
		{
			path_error( path );
		}
		
		return Value_nothing;
	}
	
	static
	Value v_close( const Value& v )
	{
		const int fd = fd_cast( v );
		
		const FileDescriptor& that = (const FileDescriptor&) v;
		
		int nok = that.close();
		
		if ( nok )
		{
			fd_error( fd );
		}
		
		return Value_nothing;
	}
	
	static
	Value v_dirname( const Value& v )
	{
		return String( plus::dirname( v.string() ) );
	}
	
	static
	Value v_dup( const Value& v )
	{
		const int fd = fd_cast( v );
		
		int new_fd = dup( fd );
		
		if ( new_fd < 0 )
		{
			fd_error( fd );
		}
		
		return FileDescriptor( new_fd );
	}
	
	static
	Value v_dup2( const Value& v )
	{
		const int old_fd = fd_cast( first( v ) );
		const int req_fd = fd_cast( rest ( v ) );
		
		int new_fd = dup2( old_fd, req_fd );
		
		if ( new_fd < 0 )
		{
			fd_error( old_fd );
		}
		
		// new_fd should be same as req_fd
		
		return FileDescriptor( new_fd );
	}
	
	static
	Value v_fstat( const Value& v )
	{
		const int fd = fd_cast( v );
		
		struct stat st;
		
		int nok = fstat( fd, &st );
		
		if ( nok )
		{
			fd_error( fd );
		}
		
		return make_stat( st );
	}
	
	static
	Value v_isatty( const Value& v )
	{
		const int fd = fd_cast( v );
		
		const int is_a_tty = isatty( fd );
		
		if ( ! is_a_tty )
		{
			/*
				Not a tty:
				
				POSIX specifies ENOTTY.
				Mac OS X yields EPERM (but ENXIO for sockets)
				Linux generates EINVAL for pipes.
			*/
			
			switch ( errno )
			{
				case ENOTTY:
				case EINVAL:
				case ENXIO:
				case EPERM:
					break;
				
				default:
					fd_error( fd );  // e.g. EBADF
			}
		}
		
		return Boolean( is_a_tty );
	}
	
	static
	Value v_link( const Value& v )
	{
		const char* target = first( v ).string().c_str();
		const char* dest   = rest ( v ).string().c_str();
		
		int nok = link( target, dest );
		
		if ( nok )
		{
			int saved_errno = errno;
			
			struct stat st;
			
			nok = stat( target, &st );
			
			if ( nok )
			{
				path_error( target );
			}
			
			path_error( dest, saved_errno );
		}
		
		return Value_nothing;
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
			
			list.append( String( name ) );
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
		
		return String( result );
	}
	
	static
	Value v_lstat( const Value& v )
	{
		const char* path = v.string().c_str();
		
		struct stat st;
		
		int nok = lstat( path, &st );
		
		if ( nok )
		{
			path_error( path );
		}
		
		return make_stat( st );
	}
	
	static
	Value v_pipe( const Value& v )
	{
		int pipe_ends[ 2 ];
		
		int nok = pipe( pipe_ends );
		
		if ( nok )
		{
			path_error( "<pipe>" );
		}
		
		return Value( FileDescriptor( pipe_ends[ 0 ] ),
		              FileDescriptor( pipe_ends[ 1 ] ) );
	}
	
	static
	Value v_read( const Value& v )
	{
		const int    fd = fd_cast( first( v ) );
		const size_t n  = rest ( v ).number().clipped();
		
		plus::string s;
		
		char* buffer = s.reset( n );
		
		ssize_t n_read = read( fd, buffer, n );
		
		if ( n_read < 0 )
		{
			fd_error( fd );
		}
		
		if ( n_read == 0 )
		{
			return empty_list;
		}
		
		return Packed( s.substr( 0, n_read ) );
	}
	
	static
	Value v_reader( const Value& v )
	{
		const char* path = v.string().c_str();
		
		/*
			Open normally -- works on devices and FIFOs, and may block.
		*/
		
		int fd = open( path, O_RDONLY );
		
		if ( fd < 0 )
		{
			path_error( path );
		}
		
		return FileDescriptor( fd, automatic );
	}
	
	static
	Value v_readlink( const Value& v )
	{
		const char* path = v.string().c_str();
		
		char buffer[ PATH_MAX ];
		
		ssize_t len = readlink( path, buffer, sizeof buffer );
		
		if ( len < 0 )
		{
			path_error( path );
		}
		
		return String( plus::string( buffer, len ) );
	}
	
	static
	Value v_realpath( const Value& v )
	{
		const char* path = v.string().c_str();
		
	#if TARGET_API_MAC_OSX  &&  ! MAC_OS_X_VERSION_10_6
		
		char buffer[ PATH_MAX ];
		
	#else
		
		char* const buffer = NULL;
		
	#endif
		
		const char* real = realpath( path, buffer );
		
		if ( real == NULL )
		{
			path_error( path );
		}
		
		const size_t len = strlen( real );
		
		if ( buffer != NULL )
		{
			return String( plus::string( real, len ) );
		}
		
		plus::string s( real, len, plus::delete_free );
		
		return String( plus::string( s.data(), len ) );
	}
	
	static
	Value v_remove( const Value& v )
	{
		const char* path = v.string().c_str();
		
		int nok = remove( path );
		
		if ( nok )
		{
			path_error( path );
		}
		
		return Value_nothing;
	}
	
	static
	Value v_rewrite( const Value& v )
	{
		const char* path = v.string().c_str();
		
		/*
			Open nonblocking in case it's a FIFO.
			Nonblocking I/O shouldn't affect writing to a regular file.
		*/
		
		int fd = open( path, O_WRONLY | O_NONBLOCK | O_CREAT | O_TRUNC, 0666 );
		
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
		
		return FileDescriptor( fd, automatic );
	}
	
	static
	Value v_stat( const Value& v )
	{
		const char* path = v.string().c_str();
		
		struct stat st;
		
		int nok = stat( path, &st );
		
		if ( nok )
		{
			path_error( path );
		}
		
		return make_stat( st );
	}
	
	static
	Value v_symlink( const Value& v )
	{
		const char* target = first( v ).string().c_str();
		const char* dest   = rest ( v ).string().c_str();
		
		int nok = symlink( target, dest );
		
		if ( nok )
		{
			path_error( dest );
		}
		
		return Value_nothing;
	}
	
	static
	Value v_touch( const Value& v )
	{
		const char* path = v.string().c_str();
		
		int nok = utime( path, 0 );  // NULL
		
		if ( nok )
		{
			path_error( path );
		}
		
		return Value_nothing;
	}
	
	static
	Value v_truncate( const Value& v )
	{
		const char*  path   = first( v ).string().c_str();
		const off_t  offset = rest ( v ).number().clipped();
		
		int nok = truncate( path, offset );
		
		if ( nok )
		{
			path_error( path );
		}
		
		return Value_nothing;
	}
	
	static
	Value v_write( const Value& v )
	{
		const int           fd = fd_cast( first( v ) );
		const plus::string& s  = rest ( v ).string();
		
		const char*  buffer = s.data();
		const size_t n      = s.size();
		
		ssize_t n_written = write( fd, buffer, n );
		
		if ( n_written < 0 )
		{
			fd_error( fd );
		}
		
		return Integer( n_written );
	}
	
	static const Type packed = packed_vtype;
	static const Type c_str = c_str_vtype;
	static const Type int32 = i32_vtype;
	static const Type uint32 = u32_vtype;
	
	static const Value fd( (Type( fd_vtype )), Op_union, int32 );
	
	static const Value c_str_x2( c_str, c_str );
	
	static const Value fd_x2( fd, fd );
	static const Value fd_u32 ( fd, uint32 );
	
	static const Value bytes( string, Op_union, packed );
	static const Value fd_bytes( fd, bytes );
	
	static const Value c_str_bytes( c_str, bytes );
	static const Value c_str_u32( c_str, uint32 );
	
	const proc_info proc_chdir    = { "chdir",    &v_chdir,    &c_str };
	const proc_info proc_close    = { "close",    &v_close,    &fd    };
	const proc_info proc_dirname  = { "dirname",  &v_dirname,  &c_str };
	const proc_info proc_dup      = { "dup",      &v_dup,      &fd    };
	const proc_info proc_dup2     = { "dup2",     &v_dup2,     &fd_x2 };
	const proc_info proc_fstat    = { "fstat",    &v_fstat,    &fd    };
	const proc_info proc_isatty   = { "isatty",   &v_isatty,   &fd    };
	const proc_info proc_link     = { "link",     &v_link,     &c_str_x2 };
	const proc_info proc_listdir  = { "listdir",  &v_listdir,  &c_str };
	const proc_info proc_load     = { "load",     &v_load,     &c_str };
	const proc_info proc_lstat    = { "lstat",    &v_lstat,    &c_str };
	const proc_info proc_pipe     = { "pipe",     &v_pipe,     &empty_list };
	const proc_info proc_read     = { "read",     &v_read,     &fd_u32 };
	const proc_info proc_reader   = { "reader",   &v_reader,   &c_str };
	const proc_info proc_readlink = { "readlink", &v_readlink, &c_str };
	const proc_info proc_realpath = { "realpath", &v_realpath, &c_str };
	const proc_info proc_remove   = { "remove",   &v_remove,   &c_str };
	const proc_info proc_rewrite  = { "rewrite",  &v_rewrite,  &c_str };
	const proc_info proc_stat     = { "stat",     &v_stat,     &c_str };
	const proc_info proc_symlink  = { "symlink",  &v_symlink,  &c_str_x2 };
	const proc_info proc_touch    = { "touch",    &v_touch,    &c_str };
	const proc_info proc_write    = { "write",    &v_write,    &fd_bytes };
	
	const proc_info proc_append   = { "append",   &v_append,   &c_str_bytes };
	const proc_info proc_truncate = { "truncate", &v_truncate, &c_str_u32 };
	
	class FIN : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.is< Unitary >()  &&  v.string() == "FIN";
			}
	};
	
	void send_data( int fd, const Value& v )
	{
		if ( v.is< FIN >() )
		{
			int nok = shutdown( fd, SHUT_WR );
			
			if ( nok )
			{
				fd_error( fd );
			}
			
			return;
		}
		
		const Packed data = pack( v );
		
		const plus::string& buffer = data.string();
		
		const size_t size = buffer.size();
		
		ssize_t n_written = write( fd, buffer.data(), size );
		
		if ( n_written < 0 )
		{
			fd_error( fd );
		}
		
		if ( n_written != size )
		{
			fd_error( fd, "partial write" );
		}
	}
	
}
}
