/*	==========
 *	Library.cc
 *	==========
 */

// Standard C++
#include <algorithm>

// Standard C/C++
#include <cstdio>
#include <cstring>

// Standard C
#include <assert.h>
#include "errno.h"
#include "signal.h"
#include <stdarg.h>
#include <stdio.h>
#include "stdlib.h"
#include "string.h"

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "sys/ioctl.h"
#include "sys/stat.h"
#include "sys/ttycom.h"
#include "sys/wait.h"
#include "unistd.h"
#include "vfork.h"

// Iota
#include "iota/environ.hh"
#include "iota/strings.hh"


// Exceptions are off here
#pragma exceptions off


	const char* sys_errlist[] =
	{
/*  0 */	"No error",
/*  1 */	"Operation not permitted",
/*  2 */	"No such file or directory",
/*  3 */	"No such process",
/*  4 */	"Interrupted system call",
/*  5 */	"Input/output error",
/*  6 */	"Device not configured",
/*  7 */	"Argument list too long",
/*  8 */	"Exec format error",
/*  9 */	"Bad file descriptor",
/* 10 */	"No child processes",
/* 11 */	"Resource deadlock avoided",
/* 12 */	"Cannot allocate memory",
/* 13 */	"Permission denied",
/* 14 */	"Bad address",
/* 15 */	"Block device required",
/* 16 */	"Device busy",
/* 17 */	"File exists",
/* 18 */	"Cross-device link",
/* 19 */	"Operation not supported by device",
/* 20 */	"Not a directory",
/* 21 */	"Is a directory",
/* 22 */	"Invalid argument",
/* 23 */	"Too many open files in system",
/* 24 */	"Too many open files",
/* 25 */	"Inappropriate ioctl for device",
/* 26 */	"Text file busy",
/* 27 */	"File too large",
/* 28 */	"No space left on device",
/* 29 */	"Illegal seek",
/* 30 */	"Read-only file system",
/* 31 */	"Too many links",
/* 32 */	"Broken pipe",
/* 33 */	"Numerical argument out of domain",
/* 34 */	"Result too large",
/* 35 */	"Operation would block",
/* 36 */	"Operation now in progress",
/* 37 */	"Operation already in progress",
/* 38 */	"Socket operation on non-socket",
/* 39 */	"Destination address required",
/* 40 */	"Message too long",
/* 41 */	"Protocol wrong type for socket",
/* 42 */	"Protocol not available",
/* 43 */	"Protocol not supported",
/* 44 */	"Socket type not supported",
/* 45 */	"Operation not supported on socket",
/* 46 */	"Protocol family not supported",
/* 47 */	"Address family not supported by protocol family",
/* 48 */	"Address already in use",
/* 49 */	"Can't assign requested address",
/* 50 */	"Network is down",
/* 51 */	"Network is unreachable",
/* 52 */	"Network dropped connection on reset",
/* 53 */	"Software caused connection abort",
/* 54 */	"Connection reset by peer",
/* 55 */	"No buffer space available",
/* 56 */	"Socket is already connected",
/* 57 */	"Socket is not connected",
/* 58 */	"Can't send after socket shutdown",
/* 59 */	"Too many references: can't splice",
/* 60 */	"Connection timed out",
/* 61 */	"Connection refused",
/* 62 */	"Too many levels of symbolic links",
/* 63 */	"File name too long",
/* 64 */	"Host is down",
/* 65 */	"No route to host",
/* 66 */	"Directory not empty",
/* 67 */	"Too many processes",
/* 68 */	"Too many users",
/* 69 */	"Disc quota exceeded",
/* 70 */	"Stale NFS file handle",
/* 71 */	"Too many levels of remote in path",
/* 72 */	"RPC struct is bad",
/* 73 */	"RPC version wrong",
/* 74 */	"RPC prog. not avail",
/* 75 */	"Program version wrong",
/* 76 */	"Bad procedure for program",
/* 77 */	"No locks available",
/* 78 */	"Function not implemented",
/* 79 */	"Inappropriate file type or format",
			NULL
	};
	
	// Length of array (not counting trailing NULL)
	int sys_nerr = sizeof sys_errlist / sizeof (const char*) - 1;
	
	char* std::strerror( int errnum )
	{
		if ( errnum < 0 )
		{
			errno = EINVAL;
			return "strerror: errnum is negative";
		}
		
		if ( errnum >= sys_nerr )
		{
			errno = EINVAL;
			return "strerror: errnum exceeds sys_nerr";
		}
		
		return const_cast< char* >( sys_errlist[ errnum ] );
	}
	
	void std::perror( const char* s )
	{
		if ( s != NULL )
		{
			std::fprintf( stderr, "%s: ", s );
		}
		
		std::fprintf( stderr, "%s\n", std::strerror( errno ) );
	}
	
	#pragma mark -
	#pragma mark ¥ stdlib ¥
	
	void abort()
	{
		(void) signal( SIGABRT, SIG_DFL );
		(void) raise( SIGABRT );
	}
	
	int raise( int sig )
	{
		return kill( getpid(), sig );
	}
	
	int system( const char* command )
	{
		if ( command == NULL )
		{
			const bool shellExists = true;
			
			return shellExists;
		}
		
		int pid = vfork();
		
		if ( pid == 0 )
		{
			const char* argv[] = { "sh", "-c", command, NULL };
			
			execv( "/bin/sh", (char**)argv );
			
			const int exitStatusForExecFailure = 127;
			
			_exit( exitStatusForExecFailure );
		}
		
		int stat = -1;
		pid_t result = waitpid( pid, &stat, 0 );
		
		//return result == -1 ? -1 : stat;
		return stat;  // Am I not clever?
	}
	
	#pragma mark -
	#pragma mark ¥ fcntl ¥
	
	int creat( const char* path, mode_t mode )
	{
		return open( path, O_CREAT | O_TRUNC | O_WRONLY, mode );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/stat ¥
	
	int mkfifo( const char* path, mode_t mode )
	{
		return mknod( path, S_IFIFO | mode, 0 );
	}
	
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	int chown( const char* /*path*/, uid_t /*owner*/, gid_t /*group*/ )
	{
		return 0;
	}
	
	int execv( const char* path, const char* const* argv )
	{
		return execve( path, argv, environ );
	}
	
	pid_t fork()
	{
		errno = ENOSYS;
		
		return -1;
	}
	
	uid_t getuid()   { return 0; }
	uid_t geteuid()  { return 0; }
	
	uid_t getgid()   { return 0; }
	uid_t getegid()  { return 0; }
	
	pid_t getpgrp()  { return getpgid( 0 ); }
	
	int isatty( int fd )
	{
		return tcgetpgrp( fd ) >= 0;
	}
	
	int link( const char* target_path, const char* link_location )
	{
		errno = ENOSYS;
		
		return -1;
	}
	
	unsigned int sleep( unsigned int seconds )
	{
		struct timespec nanoseconds = { seconds, 0 };
		
		nanosleep( &nanoseconds, &nanoseconds );
		
		return nanoseconds.tv_sec + ( nanoseconds.tv_nsec > 0 );
	}
	
	pid_t tcgetpgrp( int fd )
	{
		int pgrp = -1;
		
		int io = ioctl( fd, TIOCGPGRP, &pgrp );
		
		return pgrp;
	}
	
	int tcsetpgrp( int fd, pid_t pgrp )
	{
		return ioctl( fd, TIOCSPGRP, (int*) &pgrp );
	}
	
	static int decimal_magnitude( unsigned x )
	{
		int result = 0;
		
		while ( x > 0 )
		{
			x /= 10;
			
			++result;
		}
		
		return result;
	}
	
	static char* inscribe_decimal_backwards( unsigned x, char* p )
	{
		while ( x > 0 )
		{
			*--p = x % 10 + '0';
		}
		
		return p;
	}
	
	char* getcwd( char* buffer, size_t buffer_size )
	{
		ssize_t length = getcwd_k( buffer, buffer_size );
		
		if ( length < 0 )
		{
			return NULL;
		}
		
		if ( length + 1 > buffer_size )
		{
			errno = ERANGE;
			
			return NULL;
		}
		
		buffer[ length ] = '\0';
		
		return buffer;
	}
	
	int readlink( const char *path, char *buffer, size_t buffer_size )
	{
		return std::min< ssize_t >( readlink_k( path, buffer, buffer_size ), buffer_size );
	}
	
	ssize_t ttyname_k( int fd, char* buffer, size_t buffer_size )
	{
		if ( !isatty( fd ) )
		{
			//errno = ENOTTY;
			
			return -1;
		}
		
		char pathname[] = "/dev/fd/123456789";
		
		char* begin = pathname + STRLEN( "/dev/fd/" );
		
		char* end = begin + decimal_magnitude( fd );
		
		if ( begin < end )
		{
			inscribe_decimal_backwards( fd, end );
		}
		else
		{
			*end++ = '0';
		}
		
		*end = '\0';
		
		return readlink_k( pathname, buffer, buffer_size );
	}
	
	const char* ttyname( int fd )
	{
		static char buffer[ 256 ];  // should be enough for a terminal name
		
		ssize_t length = ttyname_k( fd, buffer, sizeof buffer );
		
		if ( length < 0 )
		{
			return NULL;
		}
		
		if ( length + 1 > sizeof buffer )
		{
			errno = ERANGE;
			
			return NULL;
		}
		
		buffer[ length ] = '\0';
		
		return buffer;
	}
	
	#pragma mark -
	#pragma mark ¥ vfork ¥
	
	static struct JmpBufNode* gJmpBufStack = NULL;
	
	struct JmpBufNode
	{
		JmpBufNode*  next;
		bool         done;
		jmp_buf      buf;
		
		JmpBufNode() : next( gJmpBufStack ), done( false )  {}
	};
	
	static JmpBufNode* GetJmpBufTopTrimmed()
	{
		JmpBufNode* top = gJmpBufStack;
		
		while ( top && top->done )
		{
			top = top->next;
			
			delete gJmpBufStack;
			
			gJmpBufStack = top;
		}
		
		return gJmpBufStack;
	}
	
	static JmpBufNode* GetJmpBufTopLeavingOneFree()
	{
		if ( gJmpBufStack == NULL )
		{
			return NULL;
		}
		
		if ( !gJmpBufStack->done )
		{
			return gJmpBufStack;
		}
		
		for ( JmpBufNode* top = gJmpBufStack->next;  top && top->done;  top = top->next )
		{
			delete gJmpBufStack;
			
			gJmpBufStack = top;
		}
		
		return gJmpBufStack;
	}
	
	jmp_buf* NewJmpBuf()
	{
		JmpBufNode* node = GetJmpBufTopLeavingOneFree();
		
		if ( node == NULL  ||  !node->done  )
		{
			node = new JmpBufNode();
			
			gJmpBufStack = node;
		}
		else
		{
			node->done = false;
		}
		
		return &node->buf;
	}
	
	void Kerosene_LongJmp( int result )
	{
		JmpBufNode* top = GetJmpBufTopTrimmed();
		
		assert( top != NULL );
		
		assert( !top->done );
		
		top->done = true;
		
		longjmp( top->buf, result );
	}
	
