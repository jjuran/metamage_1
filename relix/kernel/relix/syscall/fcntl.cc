/*
	fcntl.cc
	--------
*/

#include "relix/syscall/fcntl.hh"

// POSIX
#include <errno.h>
#include <fcntl.h>

// vfs
#include "vfs/filehandle.hh"
#include "vfs/file_descriptor.hh"

// relix-kernel
#include "relix/api/dup_fd.hh"
#include "relix/api/errno.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/api/get_fd.hh"


#ifndef O_CLOEXEC
#define O_CLOEXEC  0
#endif

#ifndef F_DUPFD_CLOEXEC
#define F_DUPFD_CLOEXEC  F_DUPFD
#endif


namespace relix
{
	
	int fcntl( int fd, int cmd, int param )
	{
		try
		{
			if ( const bool dup = (cmd | O_CLOEXEC) == F_DUPFD_CLOEXEC )
			{
				const bool close_on_exec = cmd == F_DUPFD_CLOEXEC;
				
				return dup_fd( fd,
				               first_free_fd( param ),
				               close_on_exec );
			}
			
			vfs::file_descriptor& descriptor = get_fd( fd );
			
			switch ( cmd )
			{
				case F_GETFD:
					return descriptor.will_close_on_exec();
				
				case F_SETFD:
					descriptor.set_to_close_on_exec( param );
					return 0;
				
				default:
					break;
			}
			
			vfs::filehandle& handle = *descriptor.handle;
			
			const int mask = O_APPEND | O_NONBLOCK;  // settable flags
			
			switch ( cmd )
			{
				case F_GETFL:
					return handle.get_flags();
					
				case F_SETFL:
					// Current unsettable flags plus new settable flags
					handle.set_flags( (handle.get_flags() & ~mask) | (param & mask) );
					return 0;
				
				default:
					break;
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return set_errno( EINVAL );
	}
	
}

