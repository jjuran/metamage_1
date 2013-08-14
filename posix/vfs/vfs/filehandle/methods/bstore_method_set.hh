/*
	bstore_method_set.hh
	--------------------
*/

#ifndef VFS_FILEHANDLE_METHODS_BSTOREMETHODSET_HH
#define VFS_FILEHANDLE_METHODS_BSTOREMETHODSET_HH

// POSIX
#include <sys/types.h>


namespace vfs
{
	
	class filehandle;
	
	
	typedef ssize_t (*pread_method)( filehandle*, char* buffer, size_t n, off_t offset );
	
	typedef off_t (*geteof_method)( filehandle* );
	
	typedef ssize_t (*pwrite_method)( filehandle*, const char* buffer, size_t n, off_t offset );
	
	typedef void (*seteof_method)( filehandle*, off_t length );
	
	struct bstore_method_set
	{
		pread_method   pread;
		geteof_method  geteof;
		pwrite_method  pwrite;
		seteof_method  seteof;
	};
	
}

#endif

