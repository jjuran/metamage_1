/*	==========
 *	Library.cc
 *	==========
 */

// Standard C/C++
#include <cstdio>
#include <cstring>

// Standard C
#include "errno.h"
#include "setjmp.h"
#include "signal.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "grp.h"
#include "pwd.h"
#include "sys/ioctl.h"
#include "sys/stat.h"
#include "sys/ttycom.h"
#include "sys/utsname.h"
#include "sys/wait.h"
#include "unistd.h"

// Iota
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
	const char* message   = s ? s    : "";
	const char* separator = s ? ": " : "";
	
	const char* error = std::strerror( errno );
	
	std::fprintf( stderr, "%s%s%s\n", message, separator, error );
}

#pragma mark -
#pragma mark ** signal **

typedef void (*signal_handler_t)(int);

signal_handler_t signal( int signo, signal_handler_t func )
{
	if ( func == SIG_ERR )
	{
		errno = EINVAL;
		
		return SIG_ERR;
	}
	
	struct sigaction old_action = { 0,    0, 0 };
	struct sigaction new_action = { func, 0, 0 };
	
	int status = ::sigaction( signo, &new_action, &old_action );
	
	if ( status < 0 )
	{
		return SIG_ERR;
	}
	
	return old_action.sa_handler;
}

#pragma mark -
#pragma mark ** stdlib **

void abort()
{
	struct sigaction action = { SIG_DFL, 0, 0 };
	
	(void) sigaction( SIGABRT, &action, NULL );
	
	(void) raise( SIGABRT );
}

/*
// already defined in openbsd-libc

int mkstemp( char* name )
{
	std::size_t length = std::strlen( name );
	
	if ( length < 6  ||  std::strcmp( name + length - 6, "XXXXXX" ) != 0 )
	{
		errno = EINVAL;
		
		return -1;
	}
	
	char* x = name + length - 6;
	
	std::strcpy( x, "000000" );
	
	do
	{
		int i = 5;
		
		while ( ++x[ i ] > '9' )
		{
			x[ i-- ] = '0';
			
			if ( i < 0 )
			{
				return -1;  // errno is already EEXIST
			}
		}
		
		int opened = open( name, O_RDWR | O_CREAT | O_EXCL, 0600 );
		
		if ( opened >= 0 )
		{
			return opened;
		}
	}
	while ( errno == EEXIST );
	
	return -1;
}
*/

int raise( int sig )
{
	const int killed = kill( getpid(), sig );
	
	if ( killed < 0 )
	{
		// Invalid signal number
		return killed;
	}
	
	// Allow signals to be delivered
	return kill( 1, 0 );
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
#pragma mark ** fcntl **

int creat( const char* path, mode_t mode )
{
	return open( path, O_CREAT | O_TRUNC | O_WRONLY, mode );
}

int open( const char* path, int flags, mode_t mode )
{
	return openat( AT_FDCWD, path, flags, mode );
}

#pragma mark -
#pragma mark ** grp **

struct group* getgrnam( const char *name )
{
	errno = ENOSYS;
	
	return NULL;
}

#pragma mark -
#pragma mark ** pwd **

struct passwd* getpwnam( const char* name )
{
	errno = ESRCH;
	
	return NULL;
}

struct passwd* getpwuid( uid_t uid )
{
	static struct passwd result =
	{
		"jr",
		"",
		0,
		0,
		0,
		"",
		"J. Random User",
		"/home/jr",
		"/bin/sh",
		0
	};
	
	result.pw_uid =
	result.pw_gid = uid;
	
	return &result;
}

#pragma mark -
#pragma mark ** setjmp **

#ifdef __POWERPC__

struct stack_frame
{
	stack_frame* next;
};

static asm stack_frame* get_backlink()
{
	lwz  r3,0(r1)
	lwz  r3,0(r3)
	blr
}

static asm void set_backlink( stack_frame* frame, const stack_frame* backlink )
{
	stw  r4,0(r3)
	blr
}

#endif

__sigjmp_buf_struct* __savemasktoenv( sigjmp_buf env, int savemask )
{
	sigset_t signals = 0xffffffff;
	
	if ( savemask )
	{
		int got = sigprocmask( SIG_BLOCK, NULL, &signals );
	}
	
	env->sigmask = signals;
	
#if defined( __POWERPC__ ) && defined( __MWERKS__ )
	
	env->backlink = get_backlink();
	
#endif
	
	return env;
}

void siglongjmp( sigjmp_buf env, int val )
{
#if defined( __POWERPC__ ) && defined( __MWERKS__ )
	
	stack_frame *const sp       = (stack_frame*) ((void**) env)[2];
	stack_frame *const backlink = (stack_frame*) env->backlink;
	
	set_backlink( sp, backlink );
	
#endif
	
	if ( env->sigmask != 0xffffffff )
	{
		int set = sigprocmask( SIG_SETMASK, &env->sigmask, NULL );
	}
	
	longjmp( env->jumpbuf, val );
}

#pragma mark -
#pragma mark ** stdio **

int fileno( FILE *stream )
{
	return stream->handle;
}

extern "C" FILE* __find_unopened_file();

FILE* fdopen( int fd, const char* type )
{
	int flags = ::fcntl( fd, F_GETFL, 0 );
	
	if ( flags == -1 )
	{
		return NULL;
	}
	
	switch ( type[0] )
	{
		case 'w':
		case 'a':
			if ( (flags + 1 - O_RDONLY & FWRITE) == 0 )
			{
				errno = EACCES;
				
				return NULL;
			}
		case 'r':
		default:
			break;
	}
	
	FILE* file = __find_unopened_file();
	
	if ( file == NULL )
	{
		return NULL;
	}
	
	return __handle_reopen( fd, type, file );
	
}

extern "C" int rename( const char* oldpath, const char* newpath );

int rename( const char* oldpath, const char* newpath )
{
	return renameat( AT_FDCWD, oldpath, AT_FDCWD, newpath );
}

int symlink( const char* oldpath, const char* newpath )
{
	return symlinkat( oldpath, AT_FDCWD, newpath );
}

#pragma mark -
#pragma mark ** sys/stat **

int mkfifoat( int dirfd, const char* path, mode_t mode )
{
	return mknodat( dirfd, path, S_IFIFO | mode, 0 );
}

int chmod( const char* path, mode_t mode )
{
	return fchmodat( AT_FDCWD, path, mode, 0 );
}

int lstat( const char* path, struct stat* sb )
{
	return fstatat( AT_FDCWD, path, sb, AT_SYMLINK_NOFOLLOW );
}


int stat( const char* path, struct stat* sb )
{
	return fstatat( AT_FDCWD, path, sb, 0 );
}

int mkdir( const char* path, mode_t mode )
{
	return mkdirat( AT_FDCWD, path, mode );
}

int mkfifo( const char* path, mode_t mode )
{
	return mkfifoat( AT_FDCWD, path, mode );
}

int mknod( const char* path, mode_t mode, dev_t dev )
{
	return mknodat( AT_FDCWD, path, mode, dev );
}

int rmdir( const char* path )
{
	return unlinkat( AT_FDCWD, path, AT_REMOVEDIR );
}

int futimens( int fd, const timespec times[2] )
{
	return utimensat( fd, NULL, times, 0 );
}

#pragma mark -
#pragma mark ** sys/wait **

int wait( int* stat )
{
	const pid_t any = -1;
	
	return waitpid( any, stat, 0 );
}

#pragma mark -
#pragma mark ** unistd **

int access( const char* path, int mode )
{
	return faccessat( AT_FDCWD, path, mode, 0 );
}

int chown( const char* path, uid_t owner, gid_t group )
{
	return fchownat( AT_FDCWD, path, owner, group, 0 );
}

int dup( int oldfd )
{
	return fcntl( oldfd, F_DUPFD, 0 );
}

int dup2( int oldfd, int newfd )
{
	return dup3( oldfd, newfd, DUP_DUP2 );
}

int link( const char* oldpath, const char* newpath )
{
	return linkat( AT_FDCWD, oldpath, AT_FDCWD, newpath, AT_SYMLINK_FOLLOW );
}

ssize_t readlink( const char *path, char *buffer, size_t buffer_size )
{
	const int saved_errno = errno;
	
	const ssize_t size = _readlink( path, buffer, buffer_size );
	
	if ( size == -1 )
	{
		return -1;
	}
	
	if ( size < -1 )
	{
		errno = saved_errno;  // was ERANGE
		
		return buffer_size;
	}
	
	return size;
}

ssize_t _readlink( const char *path, char *buffer, size_t buffer_size )
{
	return _readlinkat( AT_FDCWD, path, buffer, buffer_size, 0 );
}

int unlink( const char* path )
{
	return unlinkat( AT_FDCWD, path, 0 );
}

pid_t fork()
{
	errno = ENOSYS;
	
	return -1;
}

int gethostname( char* result, size_t buffer_length )
{
	struct utsname uts;
	
	int got = uname( &uts );
	
	if ( got < 0 )
	{
		return got;
	}
	
	const char* nodename = uts.nodename;
	
	const size_t name_length = std::strlen( nodename ) + 1;  // count the NUL
	
	if ( name_length > buffer_length )
	{
		errno = ENAMETOOLONG;
		
		return -1;
	}
	
	memcpy( result, nodename, name_length );
	
	return 0;
}

int getpagesize()
{
	return 8192;  // Works for now
}

uid_t getuid()   { return 0; }
uid_t geteuid()  { return 0; }

uid_t getgid()   { return 0; }
uid_t getegid()  { return 0; }

pid_t getpgrp()  { return getpgid( 0 ); }

int initgroups( const char *user, gid_t group )
{
	errno = ENOSYS;
	
	return -1;
}

int isatty( int fd )
{
	return tcgetpgrp( fd ) >= 0;
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
	
	if ( io < 0  &&  errno == EINVAL )
	{
		errno = ENOTTY;
	}
	
	return pgrp;
}

int tcsetpgrp( int fd, pid_t pgrp )
{
	return ioctl( fd, TIOCSPGRP, (int*) &pgrp );
}

ssize_t _getcwd( char* buffer, size_t buffer_size )
{
	return _realpath( ".", buffer, buffer_size );
}

char* getcwd( char* buffer, size_t buffer_size )
{
	if ( buffer_size == 0 )
	{
		errno = EINVAL;
		
		return NULL;
	}
	
	const ssize_t length = _getcwd( buffer, buffer_size - 1 );
	
	if ( length < 0 )
	{
		return NULL;
	}
	
	buffer[ length ] = '\0';
	
	return buffer;
}

int pipe( int pipefd[2] )
{
	return pipe2( pipefd, 0 );
}

ssize_t readlinkat( int dirfd, const char *path, char *buffer, size_t buffer_size )
{
	const int saved_errno = errno;
	
	const ssize_t size = _readlinkat( dirfd, path, buffer, buffer_size, 0 );
	
	if ( size < -1 )
	{
		errno = saved_errno;  // was ERANGE
		
		return buffer_size;
	}
	
	return size;
}

int setgid( gid_t gid )
{
	if ( gid != 0 )
	{
		errno = EINVAL;
		
		return -1;
	}
	
	return 0;
}

int setuid( uid_t uid )
{
	if ( uid != 0 )
	{
		errno = EINVAL;
		
		return -1;
	}
	
	return 0;
}

