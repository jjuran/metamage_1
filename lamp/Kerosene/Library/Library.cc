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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "pwd.h"
#include "sys/ioctl.h"
#include "sys/stat.h"
#include "sys/ttycom.h"
#include "sys/utsname.h"
#include "sys/wait.h"
#include "unistd.h"
#include "utime.h"

// Iota
#include "iota/strings.hh"

// TimeOff
#include "TimeOff.hh"

// Kerosene
#include "FreeTheMallocPool.h"


// Exceptions are off here
#pragma exceptions off


typedef void (*CleanupHandler)();

extern "C" void InitProc( CleanupHandler, int* );

extern "C" void InitializeCallbacks();

void InitializeCallbacks()
{
	InitProc( &FreeTheMallocPool, &errno );
}

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

int open( const char* path, int flags, mode_t mode )
{
	return openat( AT_FDCWD, path, flags, mode );
}

#pragma mark -
#pragma mark ¥ pwd ¥

struct passwd* getpwnam( const char* name )
{
	errno = ESRCH;
	
	return NULL;
}

struct passwd* getpwuid( uid_t uid )
{
	static struct passwd result =
	{
		"jruser",
		"",
		0,
		0,
		0,
		"",
		"J. Random User",
		"/",
		"/bin/sh",
		0
	};
	
	result.pw_uid =
	result.pw_gid = uid;
	
	return &result;
}

#pragma mark -
#pragma mark ¥ setjmp ¥

__sigjmp_buf_struct* __savemasktoenv( sigjmp_buf env, int savemask )
{
	sigset_t signals = 0xffffffff;
	
	if ( savemask )
	{
		int got = sigprocmask( SIG_BLOCK, NULL, &signals );
	}
	
	env->sigmask = signals;
	
	return env;
}

void siglongjmp( sigjmp_buf env, int val )
{
	if ( env->sigmask != 0xffffffff )
	{
		int set = sigprocmask( SIG_SETMASK, &env->sigmask, NULL );
	}
	
	longjmp( env->jumpbuf, val );
}

#pragma mark -
#pragma mark ¥ stdio ¥

int fileno( FILE *stream )
{
	return _fileno( stream );
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
			if ( (flags + 1 & FWRITE) == 0 )
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
#pragma mark ¥ sys/stat ¥

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

#pragma mark -
#pragma mark ¥ sys/wait ¥

int wait( int* stat )
{
	const pid_t any = -1;
	
	return waitpid( any, stat, 0 );
}

#pragma mark -
#pragma mark ¥ time ¥

/*
	
	jan	31	31	31	31
	feb	28	29	59	60
	mar	31	31	90	91
	apr	30	30	120	121
	may	31	31	151	152
	jun	30	30	181	182
	jul	31	31	212	213
	aug	31	31	243	244
	sep	30	30	273	274
	oct	31	31	304	305
	nov	30	30	334	335
	dec	31	31	365	366
	
*/

struct tm* gmtime_r( const time_t* time_p, struct tm* result )
{
	if ( time_p == NULL  ||  result == NULL )
	{
		errno = EFAULT;
		
		return NULL;
	}
	
	if ( *time_p < 0 )
	{
		errno = EINVAL;
		
		return NULL;
	}
	
	int days_since_epoch       = *time_p / (60 * 60 * 24);
	int seconds_after_midnight = *time_p % (60 * 60 * 24);
	
	result->tm_sec  = seconds_after_midnight        % 60;
	result->tm_min  = seconds_after_midnight /   60 % 60;
	result->tm_hour = seconds_after_midnight / 3600;
	
	days_since_epoch += 2 * 365 + 1;  // epoch is now 1968
	
	// Working in quad years is valid until 2099
	const int days_per_quad = 4 * 365 + 1;
	
	int quads      = days_since_epoch / days_per_quad;
	int extra_days = days_since_epoch % days_per_quad;
	
	int extra_years = ( extra_days - 1 ) / 365;
	
	int years = quads * 4 + extra_years;
	
	years += 68;  // epoch is now 1900
	
	result->tm_year = years;
	
	if ( extra_years )
	{
		extra_days -= 1;
		extra_days %= 365;
	}
	
	result->tm_yday = extra_days;
	
	result->tm_wday = (years + years / 4 + extra_days) % 7;
	
	const bool leap_year = extra_years == 0;
	
	if ( extra_days < (31 + 28 + leap_year) )
	{
		bool jan = extra_days < 31;
		
		result->tm_mday = 1 + extra_days - (jan ? 0 : 31);
		result->tm_mon  =                   jan ? 0 :  1;
	}
	else
	{
		extra_days -= 31 + 28 + leap_year;
		
		bool late = extra_days >= 153;  // after July
		
		extra_days %= 153;
		
		int month_offset = extra_days * 2 / 61;
		
		result->tm_mon = 2 + 5 * late + month_offset;
		
		result->tm_mday = 1 + extra_days - month_offset * 61 / 2 - month_offset % 2;
	}
	
	result->tm_isdst = 0;
	
	return result;
}

struct tm* gmtime( const time_t* time_p )
{
	static struct tm static_tm;
	
	return gmtime_r( time_p, &static_tm );
}

struct tm* localtime_r( const time_t* time_p, struct tm* result )
{
	if ( time_p == NULL )
	{
		return NULL;
	}
	
	long gmtDeltaField = TimeOff::GetGMTDeltaField();
	
	time_t adjusted_time = *time_p + TimeOff::GetGMTDeltaFromField( gmtDeltaField );
	
	if ( gmtime_r( &adjusted_time, result ) == NULL )
	{
		return NULL;
	}
	
	result->tm_isdst = TimeOff::GetDLSFromGMTDeltaField( gmtDeltaField );
	
	return result;
}

struct tm* localtime( const time_t* time_p )
{
	static struct tm static_tm;
	
	return localtime_r( time_p, &static_tm );
}

#pragma mark -
#pragma mark ¥ unistd ¥

int access( const char* path, int mode )
{
	return faccessat( AT_FDCWD, path, mode, 0 );
}

int chown( const char* path, uid_t owner, gid_t group )
{
	return fchownat( AT_FDCWD, path, owner, group, 0 );
}

int link( const char* oldpath, const char* newpath )
{
	return linkat( AT_FDCWD, oldpath, AT_FDCWD, newpath, 0 );
}

int readlink( const char *path, char *buffer, size_t buffer_size )
{
	const int saved_errno = errno;
	
	const ssize_t size = readlink_k( path, buffer, buffer_size );
	
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

ssize_t readlink_k( const char *path, char *buffer, size_t buffer_size )
{
	return readlinkat_k( AT_FDCWD, path, buffer, buffer_size );
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
	
	size_t name_length = std::strlen( nodename );
	
	if ( name_length > buffer_length )
	{
		errno = ENAMETOOLONG;
		
		return -1;
	}
	
	std::copy( nodename, nodename + name_length + 1, result );
	
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
		
		x /= 10;
	}
	
	return p;
}

char* getcwd( char* buffer, size_t buffer_size )
{
	if ( buffer_size == 0 )
	{
		errno = EINVAL;
		
		return NULL;
	}
	
	ssize_t length = getcwd_k( buffer, buffer_size - 1 );
	
	if ( length < 0 )
	{
		return NULL;
	}
	
	buffer[ length ] = '\0';
	
	return buffer;
}

ssize_t pread( int fd, void *buffer, size_t count, off_t offset )
{
	off_t saved_offset = lseek( fd, 0, SEEK_CUR );
	
	if ( saved_offset < 0 )
	{
		return saved_offset;
	}
	
	off_t set = lseek( fd, offset, SEEK_SET );
	
	if ( set < 0 )
	{
		return set;
	}
	
	ssize_t bytes_read = read( fd, buffer, count );
	
	if ( bytes_read < 0 )
	{
		return bytes_read;
	}
	
	set = lseek( fd, saved_offset, SEEK_SET );
	
	if ( set < 0 )
	{
		return set;
	}
	
	return bytes_read;
}

ssize_t readlinkat( int dirfd, const char *path, char *buffer, size_t buffer_size )
{
	const int saved_errno = errno;
	
	const ssize_t size = readlinkat_k( dirfd, path, buffer, buffer_size );
	
	if ( size == -1 )
	{
		return -1;
	}
	
	if ( size < -1 )
	{
		errno = saved_errno;  // was ERANGE
	}
	
	return 0;
}

char* realpath( const char *path, char *buffer )
{
	const size_t buffer_size = 4096;
	
	const int saved_errno = errno;
	
	ssize_t length = realpath_k( path, buffer, buffer_size - 1 );
	
	if ( length == -1 )
	{
		return NULL;
	}
	
	if ( length < -1 )
	{
		errno = saved_errno;  // was ERANGE
		
		length = buffer_size - 1;
	}
	
	buffer[ length ] = '\0';
	
	return buffer;
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

int ttyname_r( int fd, char* buffer, size_t buffer_size )
{
	ssize_t length = ttyname_k( fd, buffer, buffer_size - 1 );
	
	if ( length < 0 )
	{
		assert( errno != 0 );
		
		return errno;
	}
	
	buffer[ length ] = '\0';
	
	return 0;
}

char* ttyname( int fd )
{
	static char buffer[ 256 ];  // should be enough for a terminal name
	
	ssize_t length = ttyname_k( fd, buffer, sizeof buffer - 1 );
	
	if ( length < 0 )
	{
		return NULL;
	}
	
	buffer[ length ] = '\0';
	
	return buffer;
}

#pragma mark -
#pragma mark ¥ utime ¥

int utime( const char* path, const struct utimbuf *time_buffer )
{
	struct timeval a_tv = { 0 };
	struct timeval m_tv = { 0 };
	
	const timeval* access = NULL;
	const timeval* mod    = NULL;
	
	if ( time_buffer )
	{
		a_tv.tv_sec = time_buffer->actime;
		m_tv.tv_sec = time_buffer->modtime;
		
		access = &a_tv;
		mod    = &m_tv;
	}
	
	return futimesat_k( AT_FDCWD, path, access, mod, NULL, NULL );
}

