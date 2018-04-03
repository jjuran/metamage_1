/*
	fcntl.h
	-------
*/

#ifndef _FCNTL_H_
#define _FCNTL_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	#define O_NONE      0x0000
	#define O_RDONLY    0x0001
	#define O_WRONLY    0x0002
	#define O_RDWR      0x0003
	#define O_ACCMODE   0x0003
	
	#define FREAD       0x0001
	#define FWRITE      0x0002
	
	#define O_NONBLOCK  0x0004      /* no delay */
	#define O_APPEND    0x0008      /* set append mode */
	#define O_SHLOCK    0x0010      /* open with shared file lock */
	#define O_EXLOCK    0x0020      /* open with exclusive file lock */
	#define O_ASYNC     0x0040      /* signal pgrp when data ready */
	#define O_FSYNC     0x0080      /* synchronous writes */
	#define O_NOFOLLOW  0x0100      /* don't follow symlinks */
	#define O_CREAT     0x0200      /* create if nonexistant */
	#define O_TRUNC     0x0400      /* truncate to zero length */
	#define O_EXCL      0x0800      /* error if already exists */
	
	#define O_NOCTTY    0   /* don't assign controlling terminal */
	#define O_NOATIME   0   /* atime is unsupported, therefore never modified */
	
	#define O_DIRECTORY 0x4000  // matches m68k and powerpc Linux
	
	#define O_EXEC    0x00040000
	#define O_SEARCH  0x00040000
	
	#define O_CLOEXEC  0x00080000
	
	#define O_XATTR  0x40000000
	
	#define AT_FDCWD  (-100)
	
	#define AT_REMOVEDIR  1
	#define AT_REMOVEANY  2
	
	#define F_DUPFD  0
	#define F_GETFD  1
	#define F_SETFD  2
	#define F_GETFL  3
	#define F_SETFL  4
	#define F_GETOWN 5
	#define F_SETOWN 6
	#define F_GETLK  7
	#define F_SETLK  8
	#define F_SETLKW 9
	
	#define F_DUPFD_CLOEXEC  (F_DUPFD | O_CLOEXEC)
	
	#define FD_CLOEXEC  1
	
	#define F_RDLCK  1
	#define F_UNLCK  2
	#define F_WRLCK  3
	
	struct flock
	{
		off_t  l_start;
		off_t  l_len;
		pid_t  l_pid;
		short  l_type;
		short  l_whence;
	};
	
	#ifdef __cplusplus
	
	int open( const char* path, int oflag, mode_t mode = 0 );
	
	int openat( int dirfd, const char* path, int flags, mode_t mode = 0 );
	
	int fcntl( int fd, int cmd, int param );
	
	#else
	
	int open( const char* path, int oflag, ... );
	
	int openat( int dirfd, const char* path, int flags, ... );
	
	int fcntl( int fd, int cmd, ... );
	
	#endif
	
	int creat( const char* pathname, mode_t mode );
	
	ssize_t pump( int fd_in, off_t* off_in, int fd_out, off_t* off_out, size_t count, unsigned flags );
	
#ifdef __cplusplus
}
#endif

#endif
