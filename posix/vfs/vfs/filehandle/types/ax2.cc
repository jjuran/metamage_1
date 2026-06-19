/*
	ax2.cc
	------
*/

#include "vfs/filehandle/types/ax2.hh"

// ax2
#include "ax2/ax2.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"


namespace vfs
{

using namespace ax2;

namespace p7 = poseven;


struct ax2_file_extra
{
	int fd;
};

static
ssize_t ax2_pread( filehandle* file, char* p, size_t n, off_t offset )
{
	ax2_file_extra& extra = *(ax2_file_extra*) file->extra();
	
	ssize_t n_read = pread_rsrc( extra.fd, p, n, offset );
	
	return p7::throw_posix_result( n_read );
}

static
off_t ax2_geteof( filehandle* file )
{
	ax2_file_extra& extra = *(ax2_file_extra*) file->extra();
	
	off_t eof = geteof_rsrc( extra.fd );
	
	return p7::throw_posix_result( eof );
}

static
ssize_t ax2_pwrite( filehandle* file, const char* p, size_t n, off_t offset )
{
	ax2_file_extra& extra = *(ax2_file_extra*) file->extra();
	
	ssize_t n_written = pwrite_rsrc( extra.fd, p, n, offset );
	
	return p7::throw_posix_result( n_written );
}

static
void ax2_seteof( vfs::filehandle* file, off_t offset )
{
	ax2_file_extra& extra = *(ax2_file_extra*) file->extra();
	
	int nok = seteof_rsrc( extra.fd, offset );
	
	p7::throw_posix_result( nok );
}

static const bstore_method_set ax2_bstore_methods =
{
	&ax2_pread,
	&ax2_geteof,
	&ax2_pwrite,
	&ax2_seteof,
};

static const filehandle_method_set ax2_methods =
{
	&ax2_bstore_methods,
};

static
void close_ax2_file( filehandle* that )
{
	ax2_file_extra& extra = *(ax2_file_extra*) that->extra();
	
	::close( extra.fd );
}

filehandle_ptr new_ax2_rsrc_access( int flags, int fd )
{
	filehandle* result = new filehandle( NULL,
	                                     flags,
	                                     &ax2_methods,
	                                     sizeof (ax2_file_extra),
	                                     &close_ax2_file );
	
	ax2_file_extra& extra = *(ax2_file_extra*) result->extra();
	
	extra.fd = fd;
	
	return result;
}

}
