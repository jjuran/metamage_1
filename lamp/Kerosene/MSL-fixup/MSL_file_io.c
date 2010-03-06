/*	=============
 *	MSL_file_io.c
 *	=============
 */

#ifdef __MWERKS__

// POSIX
#include <fcntl.h>
#include <unistd.h>

// MSL
#include "file_io.h"

int	__open_file(const char * name, __file_modes mode, __file_handle * handle)
{
	mode_t posix_mode = 0;
	
	int fd = -1;
	
	switch ( mode.io_mode & __read_write )
	{
		case __read:
			posix_mode |= O_RDONLY;
			break;
		
		case __write:
			posix_mode |= O_WRONLY;
			break;
		
		case __read_write:
			posix_mode |= O_RDWR;
			break;
		
		default:
			break;
	}
	
	if ( mode.io_mode & __append )
	{
		posix_mode |= O_APPEND;
	}
	
	if ( mode.open_mode != __must_exist )
	{
		posix_mode |= O_CREAT;
		
		if ( mode.open_mode == __create_or_truncate )
		{
			posix_mode |= O_TRUNC;
		}
	}
	
	fd = open( name, posix_mode, 0666 );
	
	if ( fd < 0 )
	{
		return __io_error;
	}
	
	*handle = fd;
	
	return __no_io_error;
}

int __read_file(__file_handle handle, unsigned char * buffer, size_t * count, __idle_proc idle_proc)
{
	int bytes_read = read( handle, buffer, *count );
	
	if ( bytes_read < 0 )
	{
		return __io_error;
	}
	
	*count = bytes_read;
	
	if ( bytes_read == 0 )
	{
		return __io_EOF;
	}
	
	return __no_io_error;
}

int __write_file(__file_handle handle, unsigned char * buffer, size_t * count, __idle_proc idle_proc)
{
	int written = write( handle, buffer, *count );
	
	if ( written < 0 )
	{
		return __io_error;
	}
	
	*count = written;
	
	return __no_io_error;
}

int __position_file(__file_handle handle, unsigned long * position, int mode, __idle_proc idle_proc)
{
	off_t offset = lseek( handle, *position, mode );
	
	if ( offset < 0 )
	{
		return __io_error;
	}
	
	*position = offset;
	
	return __no_io_error;
}

int __close_file(__file_handle handle)
{
	int closed = close( handle );
	
	return closed < 0 ? __io_error : __no_io_error;
}

int remove( const char* name )
{
	return unlink( name );
}

#endif

