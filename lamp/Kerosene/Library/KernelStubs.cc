/*	==============
 *	KernelStubs.cc
 *	==============
 */

// Standard C
#include "errno.h"

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "signal.h"
#include "stdlib.h"
#include "sys/ioctl.h"
#include "sys/select.h"
#include "sys/socket.h"
#include "sys/stat.h"
#include "sys/utsname.h"
#include "sys/wait.h"
#include "unistd.h"

// Mac OS
#include <Threads.h>

// Nitrogen
//#include "Nitrogen/Assert.h"
//#include "Nitrogen/OSStatus.h"

// Kerosene
#include "SystemCalls.hh"


static void EnterComa()
{
	while ( true )
	{
		::YieldToAnyThread();
	}
}

static void CheckCriticalImport( void* symbol )
{
	if ( symbol == NULL )
	{
		EnterComa();
	}
}

class UnimplementedSystemCall {};

inline void CheckImportedSymbol( void* symbol, const char* name, std::size_t len )
{
	if ( symbol == NULL )
	{
		const char unimplemented[] = ": unimplemented system call\n";
		
		write( STDERR_FILENO, name,          len                      );
		write( STDERR_FILENO, unimplemented, sizeof unimplemented - 1 );
		
		throw UnimplementedSystemCall();
	}
}

#define CHECK_IMPORT(name)  CheckImportedSymbol( name##_import_, #name, sizeof #name - 1 )

#pragma export on
	
	// _import_
	
	// dirent
	DIR*    (*opendir_import_  )( const char* pathname );
	int     (*closedir_import_ )( DIR* dir );
	dirent* (*readdir_import_  )( DIR* dir );
	void    (*rewinddir_import_)( DIR* dir );
	void    (*seekdir_import_  )( DIR* dir, off_t offset );
	off_t   (*telldir_import_  )( DIR* dir );
	int     (*dirfd_import_    )( DIR* dir );
	
	// fcntl
	int (*fcntl_import_)( int fd, int cmd, int param );
	int (*open_import_ )( const char* path, int oflags, mode_t mode );
	
	// signal
	int           (*kill_import_       )( pid_t pid, int sig );
	int           (*sigaction_import_  )( int signum, const struct sigaction* act, struct sigaction* oldact );
	int           (*sigprocmask_import_)( int how, const sigset_t* set, sigset_t* oldset );
	int           (*sigpending_import_ )( sigset_t* set );
	int           (*sigsuspend_import_ )( const sigset_t* mask );
	__sig_handler (*signal_import_     )( int sig, __sig_handler func );
	
	// stdlib
	char* (*getenv_import_  )( const char* name );
	int   (*setenv_import_  )( const char* name, const char* value, int overwrite );
	int   (*putenv_import_  )( const char* string );
	void  (*unsetenv_import_)( const char* name );
	int   (*clearenv_import_)();
	
	// sys/ioctl
	int (*ioctl_import_)( int fd, unsigned long cmd, int* argp );
	
	// sys/select
	int (*select_import_)( int n, fd_set*  readfds,
	                              fd_set*  writefds,
	                              fd_set*  exceptfds, struct timeval* timeout );
	
	// sys/socket
	int     (*socket_import_     )( int domain, int type, int protocol );
	int     (*bind_import_       )( int sockfd, const struct sockaddr* name, socklen_t namelen );
	int     (*listen_import_     )( int sockfd, int backlog );
	int     (*accept_import_     )( int sockfd, struct sockaddr* addr, socklen_t* addrlen );
	int     (*connect_import_    )( int sockfd, const struct sockaddr* serv_addr, socklen_t addrlen );
	ssize_t (*send_import_       )( int s, const void* buf, size_t len, int flags );
	ssize_t (*sendto_import_     )( int s, const void* buf, size_t len, int flags, const struct sockaddr* to, socklen_t tolen );
	ssize_t (*recv_import_       )( int s, void* buf, size_t len, int flags );
	ssize_t (*recvfrom_import_   )( int s, void* buf, size_t len, int flags, struct sockaddr* from, socklen_t* fromlen );
	int     (*getsockname_import_)( int sockfd, struct sockaddr* name, socklen_t* namelen );
	int     (*getpeername_import_)( int sockfd, struct sockaddr* name, socklen_t* namelen );
	int     (*getsockopt_import_ )( int s, int level, int optname, void* optval, socklen_t* optlen );
	int     (*setsockopt_import_ )( int s, int  level, int optname, const void* optval, socklen_t optlen );
	int     (*shutdown_import_   )( int s, int how );
	
	// sys/stat
	int (*chmod_import_ )( const char *path, mode_t mode );
	int (*fchmod_import_)( int filedes, mode_t mode );
	int (*fstat_import_ )( int filedes, struct stat* buf );
	int (*lstat_import_ )( const char* file_name, struct stat* buf);
	int (*stat_import_  )( const char* file_name, struct stat* buf);
	
	// sys/utsname
	int (*uname_import_)( struct utsname* );
	
	// sys/wait
	int (*gettimeofday_import_)( struct timeval* tv, struct timezone* tz );
	
	// sys/wait
	pid_t (*waitpid_import_)( pid_t pid, int* stat_loc, int options );
	
	// unistd
	int  (*execve_import_    )( const char* path, const char* const argv[], const char* const* envp );
	void (*_exit_import_     )( int status );
	int  (*SpawnVFork_import_)();
	
	// unistd
	unsigned int (*alarm_import_    )( unsigned int seconds );
	int          (*chdir_import_    )( const char* path );
	int          (*close_import_    )( int filedes );
	int          (*copyfile_import_ )( const char* src, const char* dest );
	int          (*dup_import_      )( int filedes );
	int          (*dup2_import_     )( int filedes, int filedes2 );
	char*        (*getcwd_import_   )( char* buf, size_t size );
	pid_t        (*getpid_import_   )();
	pid_t        (*getpgid_import_  )( pid_t pid );
	pid_t        (*getppid_import_  )();
	off_t        (*lseek_import_    )( int fildes, off_t offset, int whence );
	int          (*mkdir_import_    )( const char* pathname, mode_t mode );
	int          (*rmdir_import_    )( const char* pathname );
	int          (*pause_import_    )();
	int          (*peek_import_     )( int fd, const char** buffer, size_t minBytes );
	int          (*pipe_import_     )( int filedes[ 2 ] );
	ssize_t      (*read_import_     )( int filedes, void* buf, size_t nbyte );
	int          (*readlink_import_ )( const char* path, char* buf, int len );
	int          (*rename_import_   )( const char* src, const char* dest );
	int          (*setpgid_import_  )( pid_t pid, pid_t pgid );
	pid_t        (*setsid_import_   )();
	unsigned int (*sleep_import_    )( unsigned int seconds );
	int          (*symlink_import_  )( const char* target, const char* link );
	int          (*truncate_import_ )( const char* path, off_t length );
	int          (*ftruncate_import_)( int fd, off_t length );
	const char*  (*ttyname_import_  )( int filedes );
	int          (*ttypair_import_  )( int filedes[ 2 ] );
	int          (*unlink_import_   )( const char* pathname );
	ssize_t      (*write_import_    )( int filedes, const void* buf, size_t nbyte );
	
	int*       (*ErrnoPtr_import_        )();
	char***    (*EnvironPtr_import_      )();
	OSStatus   (*AESendBlocking_import_  )( const AppleEvent* appleEvent, AppleEvent* reply );
	InetSvcRef (*InternetServices_import_)();
	OSStatus   (*Path2FSSpec_import_     )( const char* pathname, FSSpec* outFSS );
	
#pragma export reset

#if 0
#pragma export on
#endif
	
	#pragma mark -
	#pragma mark ¥ dirent ¥
	
	DIR* opendir( const char* pathname )
	{
		CHECK_IMPORT( opendir );
		
		return opendir_import_( pathname );
	}
	
	int closedir( DIR* dir )
	{
		CHECK_IMPORT( closedir );
		
		return closedir_import_( dir );
	}
	
	struct dirent* readdir( DIR* dir )
	{
		CHECK_IMPORT( readdir );
		
		return readdir_import_( dir );
	}
	
	void rewinddir( DIR* dir )
	{
		CHECK_IMPORT( rewinddir );
		
		return rewinddir_import_( dir );
	}
	
	void seekdir( DIR* dir, off_t offset )
	{
		CHECK_IMPORT( seekdir );
		
		return seekdir_import_( dir, offset );
	}
	
	off_t telldir( DIR* dir )
	{
		CHECK_IMPORT( telldir );
		
		return telldir_import_( dir );
	}
	
	int dirfd( DIR* dir )
	{
		CHECK_IMPORT( dirfd );
		
		return dirfd_import_( dir );
	}
	
	#pragma mark -
	#pragma mark ¥ fcntl ¥
	
#ifdef __cplusplus
	
	int fcntl( int fd, int cmd, int param )
	{
		CHECK_IMPORT( fcntl );
		
		return fcntl_import_( fd, cmd, param );
	}
	
	int open( const char* path, int oflags, mode_t mode )
	{
		CHECK_IMPORT( open );
		
		return open_import_( path, oflags, mode );
	}
	
#else
	
	int fcntl( int fd, int cmd, ... )
	{
		va_list va;
		
		va_start( va, cmd );
		
		int param = va_arg( va, int );
		
		va_end( va );
		
		CHECK_IMPORT( fcntl );
		
		return fcntl_import_( fd, cmd, param );
	}
	
	int open( const char* path, int oflags, ... )
	{
		va_list va;
		
		va_start( va, oflags );
		
		mode_t mode = va_arg( va, mode_t );
		
		va_end( va );
		
		CHECK_IMPORT( open );
		
		return open_import_( path, oflags, mode );
	}
	
#endif
	
	#pragma mark -
	#pragma mark ¥ signal ¥
	
	int kill( pid_t pid, int sig )
	{
		//CHECK_IMPORT( kill );
		CheckCriticalImport( kill_import_ );
		
		return kill_import_( pid, sig );
	}
	
	int sigaction( int signum, const struct sigaction* act, struct sigaction* oldact )
	{
		CHECK_IMPORT( sigaction );
		
		return sigaction_import_( signum, act, oldact );
	}
	
	int sigprocmask( int how, const sigset_t* set, sigset_t* oldset )
	{
		CHECK_IMPORT( sigprocmask );
		
		return sigprocmask_import_( how, set, oldset );
	}
	
	int sigpending( sigset_t* set )
	{
		CHECK_IMPORT( sigpending );
		
		return sigpending_import_( set );
	}
	
	int sigsuspend( const sigset_t* mask )
	{
		CHECK_IMPORT( sigsuspend );
		
		return sigsuspend_import_( mask );
	}
	
	__sig_handler signal( int sig, __sig_handler func )
	{
		//CHECK_IMPORT( signal );
		CheckCriticalImport( signal_import_ );
		
		return signal_import_( sig, func );
	}
	
	#pragma mark -
	#pragma mark ¥ stdlib ¥
	
	char* getenv( const char* name )
	{
		CHECK_IMPORT( getenv );
		
		return getenv_import_( name );
	}
	
	int setenv( const char* name, const char* value, int overwrite )
	{
		CHECK_IMPORT( setenv );
		
		return setenv_import_( name, value, overwrite );
	}
	
	int putenv( const char* string )
	{
		CHECK_IMPORT( putenv );
		
		return putenv_import_( string );
	}
	
	void unsetenv( const char* name )
	{
		CHECK_IMPORT( unsetenv );
		
		return unsetenv_import_( name );
	}
	
	int clearenv()
	{
		CHECK_IMPORT( clearenv );
		
		return clearenv_import_();
	}
	
	#pragma mark -
	#pragma mark ¥ sys/ioctl ¥
	
	int ioctl( int fd, unsigned long cmd, int* argp )
	{
		CHECK_IMPORT( ioctl );
		
		return ioctl_import_( fd, cmd, argp );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/select ¥
	
	int select( int n, fd_set*  readfds,
	                   fd_set*  writefds,
	                   fd_set*  exceptfds, struct timeval* timeout )
	{
		CHECK_IMPORT( select );
		
		return select_import_( n, readfds, writefds, exceptfds, timeout );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/socket ¥
	
	int socket( int domain, int type, int protocol )
	{
		CHECK_IMPORT( socket );
		
		return socket_import_( domain, type, protocol );
	}
	
	int bind( int sockfd, const struct sockaddr* name, socklen_t namelen )
	{
		CHECK_IMPORT( bind );
		
		return bind_import_( sockfd, name, namelen );
	}
	
	int listen( int sockfd, int backlog )
	{
		CHECK_IMPORT( listen );
		
		return listen_import_( sockfd, backlog );
	}
	
	int accept( int sockfd, struct sockaddr* addr, socklen_t* addrlen )
	{
		CHECK_IMPORT( accept );
		
		return accept_import_( sockfd, addr, addrlen );
	}
	
	int connect( int sockfd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		CHECK_IMPORT( connect );
		
		return connect_import_( sockfd, serv_addr, addrlen );
	}
	
	ssize_t send  ( int s, const void* buf, size_t len, int flags )
	{
		CHECK_IMPORT( send );
		
		return send_import_( s, buf, len, flags );
	}
	
	ssize_t sendto( int s, const void* buf, size_t len, int flags, const struct sockaddr* to, socklen_t tolen )
	{
		CHECK_IMPORT( sendto );
		
		return sendto_import_( s, buf, len, flags, to, tolen );
	}
	
	ssize_t recv( int s, void* buf, size_t len, int flags )
	{
		CHECK_IMPORT( recv );
		
		return recv_import_( s, buf, len, flags );
	}
	
	ssize_t recvfrom( int s, void* buf, size_t len, int flags, struct sockaddr* from, socklen_t* fromlen )
	{
		CHECK_IMPORT( recvfrom );
		
		return recvfrom_import_( s, buf, len, flags, from, fromlen );
	}
	
	int getsockname( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		CHECK_IMPORT( getsockname );
		
		return getsockname_import_( sockfd, name, namelen );
	}
	
	int getpeername( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		CHECK_IMPORT( getpeername );
		
		return getpeername_import_( sockfd, name, namelen );
	}
	
	int getsockopt( int s, int level, int optname, void* optval, socklen_t* optlen )
	{
		CHECK_IMPORT( getsockopt );
		
		return getsockopt_import_( s, level, optname, optval, optlen );
	}
	
	int setsockopt( int s, int  level, int optname, const void* optval, socklen_t optlen )
	{
		CHECK_IMPORT( setsockopt );
		
		return setsockopt_import_( s, level, optname, optval, optlen );
	}
	
	int shutdown( int s, int how )
	{
		CHECK_IMPORT( shutdown );
		
		return shutdown_import_( s, how );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/stat ¥
	
	int chmod( const char *path, mode_t mode )
	{
		if ( chmod_import_ == NULL )
		{
			Errno() = EINVAL;
			return -1;
		}
		
		CHECK_IMPORT( chmod );
		
		return chmod_import_( path, mode );
	}
	
	int fchmod( int filedes, mode_t mode )
	{
		if ( fchmod_import_ == NULL )
		{
			Errno() = EINVAL;
			return -1;
		}
		
		CHECK_IMPORT( fchmod );
		
		return fchmod_import_( filedes, mode );
	}
	
	int fstat( int filedes, struct stat* buf )
	{
		CHECK_IMPORT( fstat );
		
		return fstat_import_( filedes, buf );
	}
	
	int lstat( const char* file_name, struct stat* buf)
	{
		CHECK_IMPORT( lstat );
		
		return lstat_import_( file_name, buf );
	}
	
	int stat( const char* file_name, struct stat* buf)
	{
		CHECK_IMPORT( stat );
		
		return stat_import_( file_name, buf );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/utsname ¥
	
	int uname( struct utsname* uts )
	{
		CHECK_IMPORT( uname );
		
		return uname_import_( uts );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/wait ¥
	
	int gettimeofday( struct timeval* tv, struct timezone* tz )
	{
		if ( gettimeofday_import_ == NULL )
		{
			Errno() = EINVAL;
			return -1;
		}
		
		CHECK_IMPORT( gettimeofday );
		
		return gettimeofday_import_( tv, tz );
	}
	
	//int settimeofday(const struct timeval *tv , const struct timezone *tz);
	
	#pragma mark -
	#pragma mark ¥ sys/wait ¥
	
	pid_t waitpid( pid_t pid, int* stat_loc, int options )
	{
		CHECK_IMPORT( waitpid );
		
		return waitpid_import_( pid, stat_loc, options );
	}
	
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	unsigned int alarm( unsigned int seconds )
	{
		CHECK_IMPORT( alarm );
		
		return alarm_import_( seconds );
	}
	
	int chdir( const char* path )
	{
		CHECK_IMPORT( chdir );
		
		return chdir_import_( path );
	}
	
	int close( int filedes )
	{
		CHECK_IMPORT( close );
		
		return close_import_( filedes );
	}
	
	int copyfile( const char* src, const char* dest )
	{
		CHECK_IMPORT( copyfile );
		
		return copyfile_import_( src, dest );
	}
	
	int dup( int filedes )
	{
		CHECK_IMPORT( dup );
		
		return dup_import_( filedes );
	}
	
	int dup2( int filedes, int filedes2 )
	{
		CHECK_IMPORT( dup2 );
		
		return dup2_import_( filedes, filedes2 );
	}
	
	int execve_Kernel( const char* path, const char* const argv[], const char* const* envp )
	{
		CHECK_IMPORT( execve );
		
		return execve_import_( path, argv, envp );
	}
	
	void _exit_Kernel( int status )
	{
		//CHECK_IMPORT( _exit );
		CheckCriticalImport( _exit_import_ );
		
		_exit_import_( status );  // Terminates process but returns if forked
	}
	
	int SpawnVFork()
	{
		CHECK_IMPORT( SpawnVFork );
		
		return SpawnVFork_import_();
	}
	
	char* getcwd( char* buf, size_t size )
	{
		CHECK_IMPORT( getcwd );
		
		return getcwd_import_( buf, size );
	}
	
	pid_t getpid()
	{
		//CHECK_IMPORT( getpid );
		
		return getpid_import_();
	}
	
	pid_t getpgid( pid_t pid )
	{
		CHECK_IMPORT( getpgid );
		
		return getpgid_import_( pid );
	}
	
	pid_t getppid()
	{
		CHECK_IMPORT( getppid );
		
		return getppid_import_();
	}
	
	off_t lseek( int fildes, off_t offset, int whence )
	{
		CHECK_IMPORT( lseek );
		
		return lseek_import_( fildes, offset, whence );
	}
	
	int mkdir( const char* pathname, mode_t mode )
	{
		CHECK_IMPORT( mkdir );
		
		return mkdir_import_( pathname, mode );
	}
	
	int rmdir( const char* pathname )
	{
		CHECK_IMPORT( rmdir );
		
		return rmdir_import_( pathname );
	}
	
	int pause()
	{
		CHECK_IMPORT( pause );
		
		return pause_import_();
	}
	
	int peek( int fd, const char** buffer, size_t minBytes )
	{
		CHECK_IMPORT( peek );
		
		return peek_import_( fd, buffer, minBytes );
	}
	
	int pipe( int filedes[ 2 ] )
	{
		CHECK_IMPORT( pipe );
		
		return pipe_import_( filedes );
	}
	
	ssize_t read( int filedes, void* buf, size_t nbyte )
	{
		CHECK_IMPORT( read );
		
		return read_import_( filedes, buf, nbyte );
	}
	
	int readlink( const char* path, char* buf, size_t len )
	{
		CHECK_IMPORT( readlink );
		
		return readlink_import_( path, buf, len );
	}
	
	int rename( const char* src, const char* dest )
	{
		CHECK_IMPORT( rename );
		
		return rename_import_( src, dest );
	}
	
	int setpgid( pid_t pid, pid_t pgid )
	{
		CHECK_IMPORT( setpgid );
		
		return setpgid_import_( pid, pgid );
	}
	
	pid_t setsid()
	{
		CHECK_IMPORT( setsid );
		
		return setsid_import_();
	}
	
	unsigned int sleep( unsigned int seconds )
	{
		CHECK_IMPORT( sleep );
		
		return sleep_import_( seconds );
	}
	
	int symlink( const char* target, const char* link )
	{
		CHECK_IMPORT( symlink );
		
		return symlink_import_( target, link );
	}
	
	int truncate( const char* path, off_t length )
	{
		CHECK_IMPORT( truncate );
		
		return truncate_import_( path, length );
	}
	
	int ftruncate( int fd, off_t length )
	{
		CHECK_IMPORT( ftruncate );
		
		return ftruncate_import_( fd, length );
	}
	
	const char* ttyname( int filedes )
	{
		CHECK_IMPORT( ttyname );
		
		return ttyname_import_( filedes );
	}
	
	int ttypair( int filedes[ 2 ] )
	{
		CHECK_IMPORT( ttypair );
		
		return ttypair_import_( filedes );
	}
	
	int unlink( const char* pathname )
	{
		CHECK_IMPORT( unlink );
		
		return unlink_import_( pathname );
	}
	
	ssize_t write( int filedes, const void* buf, size_t nbyte )
	{
		if ( write_import_ == NULL )
		{
			// There's not much we can do.
			// write() and _exit() are currently in the same module,
			// and the chance that signal() is available is very slight.
			
			EnterComa();
		}
		
		return write_import_( filedes, buf, nbyte );
	}
	
	#pragma mark -
	#pragma mark ¥ Genie ¥
	
	int* ErrnoPtr()
	{
		CHECK_IMPORT( ErrnoPtr );
		
		int* errnoPtr = ErrnoPtr_import_();
		
		//ASSERT( errnoPtr != NULL );
		
		return errnoPtr;
	}
	
	char*** EnvironPtr()
	{
		CHECK_IMPORT( EnvironPtr );
		
		char*** environPtr = EnvironPtr_import_();
		
		//ASSERT( environPtr != NULL );
		
		return environPtr;
	}
	
	OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply )
	{
		CHECK_IMPORT( AESendBlocking );
		
		return AESendBlocking_import_( appleEvent, reply );
	}
	
	InetSvcRef InternetServices()
	{
		CHECK_IMPORT( InternetServices );
		
		return InternetServices_import_();
	}
	
	FSSpec Path2FSS( const char* pathname )
	{
		CHECK_IMPORT( Path2FSSpec );
	
		FSSpec spec;	
		//Nitrogen::ThrowOSStatus( stub( pathname, &spec ) );
		
		OSStatus err = Path2FSSpec_import_( pathname, &spec );
		
		if ( err != noErr )
		{
			spec.vRefNum = 0x8000;
			spec.parID = err;
			spec.name[0] = 0;
		}
		
		return spec;
	}
	
#if 0	
#pragma export reset
#endif

