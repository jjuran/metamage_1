/*
	stream_method_set.hh
	--------------------
*/

#ifndef VFS_FILEHANDLE_METHODS_STREAMMETHODSET_HH
#define VFS_FILEHANDLE_METHODS_STREAMMETHODSET_HH

// POSIX
#include <sys/types.h>


namespace vfs
{
	
	class filehandle;
	
	
	typedef unsigned (*poll_method)( filehandle* );
	
	typedef ssize_t (*read_method)( filehandle*, char* buffer, size_t n );
	
	typedef ssize_t (*write_method)( filehandle*, const char* buffer, size_t n );
	
	struct stream_method_set
	{
		poll_method   poll;
		read_method   read;
		write_method  write;
	};
	
}

#endif

