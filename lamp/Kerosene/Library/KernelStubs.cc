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
#include "sys/select.h"
#include "sys/socket.h"
#include "sys/stat.h"
#include "sys/wait.h"
#include "unistd.h"

#include "vfork.h"

// Nitrogen
//#include "Nitrogen/Assert.h"
//#include "Nitrogen/OSStatus.h"

// Kerosene
#include "SystemCalls.hh"


class UnimplementedSystemCall {};

inline void CheckImportedSymbol( void* symbol )
{
	if ( symbol == NULL )
	{
		throw UnimplementedSystemCall();
	}
}

#pragma export on
	
	// _import_
	
	DIR*    (*opendir_import_  )( const char* pathname );
	int     (*closedir_import_ )( DIR* dir );
	dirent* (*readdir_import_  )( DIR* dir );
	void    (*rewinddir_import_)( DIR* dir );
	void    (*seekdir_import_  )( DIR* dir, off_t offset );
	off_t   (*telldir_import_  )( DIR* );
	
	int (*fcntl_import_)( int fd, int cmd, int param );
	int (*open_import_ )( const char* path, int oflags, mode_t mode );
	
	int           (*kill_import_       )( pid_t pid, int sig );
	int           (*sigaction_import_  )( int signum, const struct sigaction* act, struct sigaction* oldact );
	int           (*sigprocmask_import_)( int how, const sigset_t* set, sigset_t* oldset );
	int           (*sigpending_import_ )( sigset_t* set );
	int           (*sigsuspend_import_ )( const sigset_t* mask );
	__sig_handler (*signal_import_     )( int sig, __sig_handler func );
	
	char* (*getenv_import_  )( const char* name );
	int   (*setenv_import_  )( const char* name, const char* value, int overwrite );
	int   (*putenv_import_  )( const char* string );
	void  (*unsetenv_import_)( const char* name );
	int   (*clearenv_import_)();
	
	int (*select_import_)( int n, fd_set*  readfds,
	                              fd_set*  writefds,
	                              fd_set*  exceptfds, struct timeval* timeout );
	
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
	
	int (*chmod_import_ )( const char *path, mode_t mode );
	int (*fchmod_import_)( int filedes, mode_t mode );
	int (*fstat_import_ )( int filedes, struct stat* buf );
	int (*lstat_import_ )( const char* file_name, struct stat* buf);
	int (*stat_import_  )( const char* file_name, struct stat* buf);
	
	int (*gettimeofday_import_)( struct timeval* tv, struct timezone* tz );
	
	pid_t (*waitpid_import_)( pid_t pid, int* stat_loc, int options );
	
	int  (*execve_import_    )( const char* path, const char* const argv[], const char* const* envp );
	void (*_exit_import_     )( int status );
	int  (*SpawnVFork_import_)();
	
	int          (*chdir_import_   )( const char* path );
	int          (*close_import_   )( int filedes );
	int          (*copyfile_import_)( const char* src, const char* dest );
	int          (*dup_import_     )( int filedes );
	int          (*dup2_import_    )( int filedes, int filedes2 );
	char*        (*getcwd_import_  )( char* buf, size_t size );
	pid_t        (*getpid_import_  )();
	pid_t        (*getpgid_import_ )( pid_t pid );
	pid_t        (*getppid_import_ )();
	off_t        (*lseek_import_   )( int fildes, off_t offset, int whence );
	int          (*mkdir_import_   )( const char* pathname, mode_t mode );
	int          (*rmdir_import_   )( const char* pathname );
	int          (*pause_import_   )();
	int          (*peek_import_    )( int fd, const char** buffer, size_t minBytes );
	int          (*pipe_import_    )( int filedes[ 2 ] );
	ssize_t      (*read_import_    )( int filedes, void* buf, size_t nbyte );
	int          (*rename_import_  )( const char* src, const char* dest );
	int          (*setctty_import_ )( int filedes );
	int          (*setpgid_import_ )( pid_t pid, pid_t pgid );
	pid_t        (*setsid_import_  )();
	unsigned int (*sleep_import_   )( unsigned int seconds );
	const char*  (*ttyname_import_ )( int filedes );
	int          (*ttypair_import_ )( int filedes[ 2 ] );
	int          (*unlink_import_  )( const char* pathname );
	ssize_t      (*write_import_   )( int filedes, const void* buf, size_t nbyte );
	
	int*       (*ErrnoPtr_import_        )();
	char***    (*EnvironPtr_import_      )();
	OSStatus   (*AESendBlocking_import_  )( const AppleEvent* appleEvent, AppleEvent* reply );
	InetSvcRef (*InternetServices_import_)();
	void       (*PrintPS_import_         )();
	OSStatus   (*Path2FSSpec_import_     )( const char* pathname, FSSpec* outFSS );
	
#pragma export reset

#if 0
#pragma export on
#endif
	
	#pragma mark -
	#pragma mark ¥ dirent ¥
	
	DIR* opendir( const char* pathname )
	{
		CheckImportedSymbol( opendir_import_ );
		
		return opendir_import_( pathname );
	}
	
	int closedir( DIR* dir )
	{
		CheckImportedSymbol( closedir_import_ );
		
		return closedir_import_( dir );
	}
	
	struct dirent* readdir( DIR* dir )
	{
		CheckImportedSymbol( readdir_import_ );
		
		return readdir_import_( dir );
	}
	
	void rewinddir( DIR* dir )
	{
		CheckImportedSymbol( rewinddir_import_ );
		
		return rewinddir_import_( dir );
	}
	
	void seekdir( DIR* dir, off_t offset )
	{
		CheckImportedSymbol( seekdir_import_ );
		
		return seekdir_import_( dir, offset );
	}
	
	off_t telldir( DIR* dir )
	{
		CheckImportedSymbol( telldir_import_ );
		
		return telldir_import_( dir );
	}
	
	#pragma mark -
	#pragma mark ¥ fcntl ¥
	
	int fcntl( int fd, int cmd, int param )
	{
		CheckImportedSymbol( fcntl_import_ );
		
		return fcntl_import_( fd, cmd, param );
	}
	
	int open( const char* path, int oflags, mode_t mode )
	{
		CheckImportedSymbol( open_import_ );
		
		return open_import_( path, oflags, mode );
	}
	
	#pragma mark -
	#pragma mark ¥ signal ¥
	
	int kill( pid_t pid, int sig )
	{
		CheckImportedSymbol( kill_import_ );
		
		return kill_import_( pid, sig );
	}
	
	int sigaction( int signum, const struct sigaction* act, struct sigaction* oldact )
	{
		CheckImportedSymbol( sigaction_import_ );
		
		return sigaction_import_( signum, act, oldact );
	}
	
	int sigprocmask( int how, const sigset_t* set, sigset_t* oldset )
	{
		CheckImportedSymbol( sigprocmask_import_ );
		
		return sigprocmask_import_( how, set, oldset );
	}
	
	int sigpending( sigset_t* set )
	{
		CheckImportedSymbol( sigpending_import_ );
		
		return sigpending_import_( set );
	}
	
	int sigsuspend( const sigset_t* mask )
	{
		CheckImportedSymbol( sigsuspend_import_ );
		
		return sigsuspend_import_( mask );
	}
	
	__sig_handler signal( int sig, __sig_handler func )
	{
		CheckImportedSymbol( signal_import_ );
		
		return signal_import_( sig, func );
	}
	
	#pragma mark -
	#pragma mark ¥ stdlib ¥
	
	char* getenv( const char* name )
	{
		CheckImportedSymbol( getenv_import_ );
		
		return getenv_import_( name );
	}
	
	int setenv( const char* name, const char* value, int overwrite )
	{
		CheckImportedSymbol( setenv_import_ );
		
		return setenv_import_( name, value, overwrite );
	}
	
	int putenv( const char* string )
	{
		CheckImportedSymbol( putenv_import_ );
		
		return putenv_import_( string );
	}
	
	void unsetenv( const char* name )
	{
		CheckImportedSymbol( unsetenv_import_ );
		
		return unsetenv_import_( name );
	}
	
	int clearenv()
	{
		CheckImportedSymbol( clearenv_import_ );
		
		return clearenv_import_();
	}
	
	#pragma mark -
	#pragma mark ¥ sys/select ¥
	
	int select( int n, fd_set*  readfds,
	                   fd_set*  writefds,
	                   fd_set*  exceptfds, struct timeval* timeout )
	{
		CheckImportedSymbol( select_import_ );
		
		return select_import_( n, readfds, writefds, exceptfds, timeout );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/socket ¥
	
	int socket( int domain, int type, int protocol )
	{
		CheckImportedSymbol( socket_import_ );
		
		return socket_import_( domain, type, protocol );
	}
	
	int bind( int sockfd, const struct sockaddr* name, socklen_t namelen )
	{
		CheckImportedSymbol( bind_import_ );
		
		return bind_import_( sockfd, name, namelen );
	}
	
	int listen( int sockfd, int backlog )
	{
		CheckImportedSymbol( listen_import_ );
		
		return listen_import_( sockfd, backlog );
	}
	
	int accept( int sockfd, struct sockaddr* addr, socklen_t* addrlen )
	{
		CheckImportedSymbol( accept_import_ );
		
		return accept_import_( sockfd, addr, addrlen );
	}
	
	int connect( int sockfd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		CheckImportedSymbol( connect_import_ );
		
		return connect_import_( sockfd, serv_addr, addrlen );
	}
	
	ssize_t send  ( int s, const void* buf, size_t len, int flags )
	{
		CheckImportedSymbol( send_import_ );
		
		return send_import_( s, buf, len, flags );
	}
	
	ssize_t sendto( int s, const void* buf, size_t len, int flags, const struct sockaddr* to, socklen_t tolen )
	{
		CheckImportedSymbol( sendto_import_ );
		
		return sendto_import_( s, buf, len, flags, to, tolen );
	}
	
	ssize_t recv( int s, void* buf, size_t len, int flags )
	{
		CheckImportedSymbol( recv_import_ );
		
		return recv_import_( s, buf, len, flags );
	}
	
	ssize_t recvfrom( int s, void* buf, size_t len, int flags, struct sockaddr* from, socklen_t* fromlen )
	{
		CheckImportedSymbol( recvfrom_import_ );
		
		return recvfrom_import_( s, buf, len, flags, from, fromlen );
	}
	
	int getsockname( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		CheckImportedSymbol( getsockname_import_ );
		
		return getsockname_import_( sockfd, name, namelen );
	}
	
	int getpeername( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		CheckImportedSymbol( getpeername_import_ );
		
		return getpeername_import_( sockfd, name, namelen );
	}
	
	int getsockopt( int s, int level, int optname, void* optval, socklen_t* optlen )
	{
		CheckImportedSymbol( getsockopt_import_ );
		
		return getsockopt_import_( s, level, optname, optval, optlen );
	}
	
	int setsockopt( int s, int  level, int optname, const void* optval, socklen_t optlen )
	{
		CheckImportedSymbol( setsockopt_import_ );
		
		return setsockopt_import_( s, level, optname, optval, optlen );
	}
	
	int shutdown( int s, int how )
	{
		CheckImportedSymbol( shutdown_import_ );
		
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
		
		CheckImportedSymbol( chmod_import_ );
		
		return chmod_import_( path, mode );
	}
	
	int fchmod( int filedes, mode_t mode )
	{
		if ( fchmod_import_ == NULL )
		{
			Errno() = EINVAL;
			return -1;
		}
		
		CheckImportedSymbol( fchmod_import_ );
		
		return fchmod_import_( filedes, mode );
	}
	
	int fstat( int filedes, struct stat* buf )
	{
		CheckImportedSymbol( fstat_import_ );
		
		return fstat_import_( filedes, buf );
	}
	
	int lstat( const char* file_name, struct stat* buf)
	{
		CheckImportedSymbol( lstat_import_ );
		
		return lstat_import_( file_name, buf );
	}
	
	int stat( const char* file_name, struct stat* buf)
	{
		CheckImportedSymbol( stat_import_ );
		
		return stat_import_( file_name, buf );
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
		
		CheckImportedSymbol( gettimeofday_import_ );
		
		return gettimeofday_import_( tv, tz );
	}
	
	//int settimeofday(const struct timeval *tv , const struct timezone *tz);
	
	#pragma mark -
	#pragma mark ¥ sys/wait ¥
	
	pid_t waitpid( pid_t pid, int* stat_loc, int options )
	{
		CheckImportedSymbol( waitpid_import_ );
		
		return waitpid_import_( pid, stat_loc, options );
	}
	
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	int chdir( const char* path )
	{
		CheckImportedSymbol( chdir_import_ );
		
		return chdir_import_( path );
	}
	
	int close( int filedes )
	{
		CheckImportedSymbol( close_import_ );
		
		return close_import_( filedes );
	}
	
	int copyfile( const char* src, const char* dest )
	{
		CheckImportedSymbol( copyfile_import_ );
		
		return copyfile_import_( src, dest );
	}
	
	int dup( int filedes )
	{
		CheckImportedSymbol( dup_import_ );
		
		return dup_import_( filedes );
	}
	
	int dup2( int filedes, int filedes2 )
	{
		CheckImportedSymbol( dup2_import_ );
		
		return dup2_import_( filedes, filedes2 );
	}
	
	int execve_Kernel( const char* path, const char* const argv[], const char* const* envp )
	{
		CheckImportedSymbol( execve_import_ );
		
		return execve_import_( path, argv, envp );
	}
	
	void _exit_Kernel( int status )
	{
		CheckImportedSymbol( _exit_import_ );
		
		_exit_import_( status );  // Terminates process but returns if forked
	}
	
	int SpawnVFork()
	{
		CheckImportedSymbol( SpawnVFork_import_ );
		
		return SpawnVFork_import_();
	}
	
	char* getcwd( char* buf, size_t size )
	{
		CheckImportedSymbol( getcwd_import_ );
		
		return getcwd_import_( buf, size );
	}
	
	pid_t getpid()
	{
		CheckImportedSymbol( getpid_import_ );
		
		return getpid_import_();
	}
	
	pid_t getpgid( pid_t pid )
	{
		CheckImportedSymbol( getpgid_import_ );
		
		return getpgid_import_( pid );
	}
	
	pid_t getppid()
	{
		CheckImportedSymbol( getppid_import_ );
		
		return getppid_import_();
	}
	
	off_t lseek( int fildes, off_t offset, int whence )
	{
		CheckImportedSymbol( lseek_import_ );
		
		return lseek_import_( fildes, offset, whence );
	}
	
	int mkdir( const char* pathname, mode_t mode )
	{
		CheckImportedSymbol( mkdir_import_ );
		
		return mkdir_import_( pathname, mode );
	}
	
	int rmdir( const char* pathname )
	{
		CheckImportedSymbol( rmdir_import_ );
		
		return rmdir_import_( pathname );
	}
	
	int pause()
	{
		CheckImportedSymbol( pause_import_ );
		
		return pause_import_();
	}
	
	int peek( int fd, const char** buffer, size_t minBytes )
	{
		CheckImportedSymbol( peek_import_ );
		
		return peek_import_( fd, buffer, minBytes );
	}
	
	int pipe( int filedes[ 2 ] )
	{
		CheckImportedSymbol( pipe_import_ );
		
		return pipe_import_( filedes );
	}
	
	ssize_t read( int filedes, void* buf, size_t nbyte )
	{
		CheckImportedSymbol( read_import_ );
		
		return read_import_( filedes, buf, nbyte );
	}
	
	int rename( const char* src, const char* dest )
	{
		CheckImportedSymbol( rename_import_ );
		
		return rename_import_( src, dest );
	}
	
	int setctty( int filedes )
	{
		CheckImportedSymbol( setctty_import_ );
		
		return setctty_import_( filedes );
	}
	
	int setpgid( pid_t pid, pid_t pgid )
	{
		CheckImportedSymbol( setpgid_import_ );
		
		return setpgid_import_( pid, pgid );
	}
	
	pid_t setsid()
	{
		CheckImportedSymbol( setsid_import_ );
		
		return setsid_import_();
	}
	
	unsigned int sleep( unsigned int seconds )
	{
		CheckImportedSymbol( sleep_import_ );
		
		return sleep_import_( seconds );
	}
	
	const char* ttyname( int filedes )
	{
		CheckImportedSymbol( ttyname_import_ );
		
		return ttyname_import_( filedes );
	}
	
	int ttypair( int filedes[ 2 ] )
	{
		CheckImportedSymbol( ttypair_import_ );
		
		return ttypair_import_( filedes );
	}
	
	int unlink( const char* pathname )
	{
		CheckImportedSymbol( unlink_import_ );
		
		return unlink_import_( pathname );
	}
	
	ssize_t write( int filedes, const void* buf, size_t nbyte )
	{
		CheckImportedSymbol( write_import_ );
		
		return write_import_( filedes, buf, nbyte );
	}
	
	#pragma mark -
	#pragma mark ¥ Genie ¥
	
	int* ErrnoPtr()
	{
		CheckImportedSymbol( ErrnoPtr_import_ );
		
		int* errnoPtr = ErrnoPtr_import_();
		
		//ASSERT( errnoPtr != NULL );
		
		return errnoPtr;
	}
	
	char*** EnvironPtr()
	{
		CheckImportedSymbol( EnvironPtr_import_ );
		
		char*** environPtr = EnvironPtr_import_();
		
		//ASSERT( environPtr != NULL );
		
		return environPtr;
	}
	
	OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply )
	{
		CheckImportedSymbol( AESendBlocking_import_ );
		
		return AESendBlocking_import_( appleEvent, reply );
	}
	
	InetSvcRef InternetServices()
	{
		CheckImportedSymbol( InternetServices_import_ );
		
		return InternetServices_import_();
	}
	
	void PrintPS()
	{
		CheckImportedSymbol( PrintPS_import_ );
		
		PrintPS_import_();
	}
	
	FSSpec Path2FSS( const char* pathname )
	{
		CheckImportedSymbol( Path2FSSpec_import_ );
	
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

