/*
	posix.cc
	--------
*/

#include "vfs/filehandle/types/posix.hh"

// poseven
#include "poseven/functions/fstat.hh"
#include "poseven/functions/ftruncate.hh"
#include "poseven/functions/pread.hh"
#include "poseven/functions/pwrite.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	struct posix_file_extra
	{
		int fd;
	};
	
	static ssize_t posix_pread( filehandle* file, char* buffer, size_t n, off_t offset )
	{
		posix_file_extra& extra = *(posix_file_extra*) file->extra();
		
		const p7::fd_t fd = p7::fd_t( extra.fd );
		
		return p7::pread( fd, buffer, n, offset );
	}
	
	static off_t posix_geteof( filehandle* file )
	{
		posix_file_extra& extra = *(posix_file_extra*) file->extra();
		
		const p7::fd_t fd = p7::fd_t( extra.fd );
		
		struct stat sb = p7::fstat( fd );
		
		return sb.st_size;
	}
	
	static ssize_t posix_pwrite( filehandle* file, const char* buffer, size_t n, off_t offset )
	{
		posix_file_extra& extra = *(posix_file_extra*) file->extra();
		
		const p7::fd_t fd = p7::fd_t( extra.fd );
		
		return p7::pwrite( fd, buffer, n, offset );
	}
	
	static void posix_seteof( vfs::filehandle* file, off_t offset )
	{
		posix_file_extra& extra = *(posix_file_extra*) file->extra();
		
		const p7::fd_t fd = p7::fd_t( extra.fd );
		
		p7::ftruncate( fd, offset );
	}
	
	static const bstore_method_set posix_bstore_methods =
	{
		&posix_pread,
		&posix_geteof,
		&posix_pwrite,
		&posix_seteof,
	};
	
	static ssize_t posix_read( vfs::filehandle* file, char* buffer, size_t n )
	{
		posix_file_extra& extra = *(posix_file_extra*) file->extra();
		
		const p7::fd_t fd = p7::fd_t( extra.fd );
		
		return p7::read( fd, buffer, n );
	}
	
	static ssize_t posix_write( vfs::filehandle* file, const char* buffer, size_t n )
	{
		posix_file_extra& extra = *(posix_file_extra*) file->extra();
		
		const p7::fd_t fd = p7::fd_t( extra.fd );
		
		return p7::write( fd, buffer, n );
	}
	
	static const stream_method_set posix_stream_methods =
	{
		NULL,
		&posix_read,
		&posix_write,
	};
	
	static const filehandle_method_set posix_methods =
	{
		&posix_bstore_methods,
		NULL,
		&posix_stream_methods,
	};
	
	static void close_posix_file( filehandle* that )
	{
		posix_file_extra& extra = *(posix_file_extra*) that->extra();
		
		::close( extra.fd );
	}
	
	filehandle_ptr new_posix_fd( int flags, int fd )
	{
		filehandle* result = new filehandle( NULL,
		                                     flags,
		                                     &posix_methods,
		                                     sizeof (posix_file_extra),
		                                     &close_posix_file );
		
		posix_file_extra& extra = *(posix_file_extra*) result->extra();
		
		extra.fd = fd;
		
		return result;
	}
	
}
