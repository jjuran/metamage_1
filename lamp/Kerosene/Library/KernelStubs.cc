/*	==============
 *	KernelStubs.cc
 *	==============
 */

// Standard C
#include "errno.h"

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "netdb.h"
#include "signal.h"
#include "stdlib.h"
#include "sys/ioctl.h"
#include "sys/ptrace.h"
#include "sys/select.h"
#include "sys/socket.h"
#include "sys/stat.h"
#include "sys/times.h"
#include "sys/utsname.h"
#include "sys/wait.h"
#include "unistd.h"
#include "vfork.h"

// Convergence
#include "clearenv.h"
#include "copyfile.hh"
#include "fork_and_exit.hh"

// Mac OS
#include <LowMem.h>
#include <OpenTransportProviders.h>
#include <Sound.h>
#include <Threads.h>

// Kerosene
#include "SystemCalls.hh"
#include "FreeTheMallocPool.h"


extern "C" int rename( const char* from, const char* to );

extern "C" int ttypair( int filedes[ 2 ] );


#pragma exceptions off


typedef void (*CleanupHandler)();

#if TARGET_CPU_68K

extern "C" void InitProc( CleanupHandler );

static void SetCleanupHandler()
{
	static bool beenHere = false;
	
	if ( !beenHere )
	{
		beenHere = true;
		
		InitProc( FreeTheMallocPool );
	}
}

#endif

static void EnterComa()
{
	::SysBeep( 30 );
	
	while ( true )
	{
		::YieldToAnyThread();
	}
}

#define STR_LEN( str )  "" str, (sizeof str - 1)

static void CheckCriticalImport( void* symbol, const char* name, std::size_t len )
{
	if ( symbol == NULL )
	{
		write( STDERR_FILENO, name, len );
		
		write( STDERR_FILENO, STR_LEN( ": CRITICAL unimplemented system call!\n" ) );
		
		EnterComa();
	}
}

inline void CheckImportedSymbol( void* symbol, const char* name, std::size_t len )
{
	if ( symbol == NULL )
	{
		write( STDERR_FILENO, name, len );
		
		write( STDERR_FILENO, STR_LEN( ": unimplemented system call\n" ) );
		
		raise( SIGSYS );
	}
}

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
	
	// netdb
	struct hostent* (*gethostbyname_import_)( const char* name );
	
	OSStatus (*OTInetMailExchange_Kernel_import_)( char*, UInt16*, InetMailExchange* );
	
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
	
	// sys/ptrace
	int (*ptrace_import_)( int request, pid_t pid, void* addr, int data );
	
	// sys/select
	int (*select_import_)( int n, fd_set*  readfds,
	                              fd_set*  writefds,
	                              fd_set*  exceptfds, struct timeval* timeout );
	
	// sys/socket
	int     (*socketpair_import_ )( int domain, int type, int protocol, int fds[2] );
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
	
	// sys/times
	clock_t (*times_import_)( struct tms* );
	
	// sys/utsname
	int (*uname_import_)( struct utsname* );
	
	// sys/wait
	int (*gettimeofday_import_)( struct timeval* tv, struct timezone* tz );
	
	// sys/wait
	pid_t (*waitpid_import_)( pid_t pid, int* stat_loc, int options );
	
	// unistd
	int  (*execve_import_       )( const char* path, const char* const argv[], const char* const* envp );
	void (*_exit_import_        )( int status );
	int  (*fork_and_exit_import_)( int status );
	int  (*SpawnVFork_import_   )( void (*LongJmp)( int ) );
	
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
	int          (*readlink_import_ )( const char* path, char* buf, size_t len );
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
	
	void       (*InitProc_import_        )( CleanupHandler );
	int*       (*ErrnoPtr_import_        )();
	OSStatus   (*AESendBlocking_import_  )( const AppleEvent* appleEvent, AppleEvent* reply );
	OSStatus   (*Path2FSSpec_import_     )( const char* pathname, FSSpec* outFSS );
	
#pragma export reset

OSStatus Path2FSSpec( const char* pathname, FSSpec* outFSS );

namespace
{
	
	template < class SysProcPtr >
	inline SysProcPtr SystemCall_Cast( SysProcPtr, void* proc )
	{
		return reinterpret_cast< SysProcPtr >( proc );
	}
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	static void* GetSystemCallFunctionPtr( const char* name )
	{
		typedef void* (*Getter)( const char* );
		
		static Getter getPtr = *reinterpret_cast< Getter* >( LMGetApplScratch() );
		
		return getPtr( name );
	}
	
	inline void LoadErrno()
	{
		*(void**)LMGetToolScratch() = &errno;
	}
	
	#define LOAD_SYMBOL( syscall )  (syscall ## _import_ = SystemCall_Cast( syscall, GetSystemCallFunctionPtr( #syscall ) ))
	
	#define INVOKE_COMMON( syscall, args )  ( SetCleanupHandler(), LoadErrno(), syscall ## _import_ args )
	
#else
	
	#define LOAD_SYMBOL( syscall )  (syscall ## _import_)
	
	#define INVOKE_COMMON( syscall, args )  ( syscall ## _import_ args )
	
#endif
	
	#define CHECK_CRITICAL( syscall )  (LOAD_SYMBOL( syscall ), CheckCriticalImport( syscall##_import_, #syscall, sizeof #syscall - 1 ))
	
	#define CHECK_IMPORT( syscall )    (LOAD_SYMBOL( syscall ), CheckImportedSymbol( syscall##_import_, #syscall, sizeof #syscall - 1 ))
	
	#define INVOKE_CRITICAL( syscall, args )  ( CHECK_CRITICAL( syscall ), INVOKE_COMMON( syscall, args ) )
	
	#define INVOKE( syscall, args )  ( CHECK_IMPORT( syscall ), INVOKE_COMMON( syscall, args ) )
	
}

#if 0
#pragma export on
#endif
	
	#pragma mark -
	#pragma mark ¥ dirent ¥
	
	DIR* opendir( const char* pathname )
	{
		return INVOKE( opendir, ( pathname ) );
	}
	
	int closedir( DIR* dir )
	{
		return INVOKE( closedir, ( dir ) );
	}
	
	struct dirent* readdir( DIR* dir )
	{
		return INVOKE( readdir, ( dir ) );
	}
	
	void rewinddir( DIR* dir )
	{
		return INVOKE( rewinddir, ( dir ) );
	}
	
	void seekdir( DIR* dir, off_t offset )
	{
		return INVOKE( seekdir, ( dir, offset ) );
	}
	
	off_t telldir( DIR* dir )
	{
		return INVOKE( telldir, ( dir ) );
	}
	
	int dirfd( DIR* dir )
	{
		return INVOKE( dirfd, ( dir ) );
	}
	
	#pragma mark -
	#pragma mark ¥ fcntl ¥
	
#ifdef __cplusplus
	
	int fcntl( int fd, int cmd, int param )
	{
		return INVOKE( fcntl, ( fd, cmd, param ) );
	}
	
	int open( const char* path, int oflags, mode_t mode )
	{
		return INVOKE( open, ( path, oflags, mode ) );
	}
	
#else
	
	int fcntl( int fd, int cmd, ... )
	{
		va_list va;
		
		va_start( va, cmd );
		
		int param = va_arg( va, int );
		
		va_end( va );
		
		return INVOKE( fcntl, ( fd, cmd, param ) );
	}
	
	int open( const char* path, int oflags, ... )
	{
		va_list va;
		
		va_start( va, oflags );
		
		mode_t mode = va_arg( va, mode_t );
		
		va_end( va );
		
		return INVOKE( open, ( path, oflags, mode ) );
	}
	
#endif
	
	#pragma mark -
	#pragma mark ¥ netdb ¥
	
	void endhostent()
	{
	}
	
	void endprotoent()
	{
	}
	
	void endservent()
	{
	}
	
	/*
	struct hostent* gethostbyaddr( const void*, size_t, int )
	{
		return NULL;
	}
	*/
	
	struct hostent* gethostbyname( const char* name )
	{
		return INVOKE( gethostbyname, ( name ) );
	}
	
	struct hostent* gethostent()
	{
		return NULL;
	}
	
	struct protoent* getprotobyname( const char* name )
	{
		return NULL;
	}
	
	struct protoent* getprotobynumber( int )
	{
		return NULL;
	}
	
	struct protoent* getprotoent()
	{
		return NULL;
	}
	
	struct servent* getservbyname( const char*, const char* )
	{
		return NULL;
	}
	
	struct servent* getservbyport( int, const char* )
	{
		return NULL;
	}
	
	struct servent* getservent()
	{
		return NULL;
	}
	
	void herror( const char* )
	{
	}
	
	/*
	char* hstrerror( int )
	{
		return "";
	}
	*/
	
	void setprotoent( int )
	{
	}
	
	void setservent( int )
	{
	}
	
	OSStatus OTInetMailExchange_Kernel( char* domain, UInt16* count, InetMailExchange* result )
	{
		return INVOKE( OTInetMailExchange_Kernel, ( domain, count, result ) );
	}
	
	// Override ::OTInetMailExchange() to call into the kernel, so we can use
	// our own SharedOpenTransport and InetSvcRef.
	pascal OSStatus OTInetMailExchange( InetSvcRef, char* domain, UInt16* count, InetMailExchange* result )
	{
		return OTInetMailExchange_Kernel( domain, count, result );
	}
	
	#pragma mark -
	#pragma mark ¥ signal ¥
	
	int kill( pid_t pid, int sig )
	{
		return INVOKE_CRITICAL( kill, ( pid, sig ) );
	}
	
	int sigaction( int signum, const struct sigaction* act, struct sigaction* oldact )
	{
		return INVOKE( sigaction, ( signum, act, oldact ) );
	}
	
	int sigprocmask( int how, const sigset_t* set, sigset_t* oldset )
	{
		return INVOKE( sigprocmask, ( how, set, oldset ) );
	}
	
	int sigpending( sigset_t* set )
	{
		return INVOKE( sigpending, ( set ) );
	}
	
	int sigsuspend( const sigset_t* mask )
	{
		return INVOKE( sigsuspend, ( mask ) );
	}
	
	__sig_handler signal( int sig, __sig_handler func )
	{
		return INVOKE_CRITICAL( signal, ( sig, func ) );
	}
	
	#pragma mark -
	#pragma mark ¥ stdlib ¥
	
	char* getenv( const char* name )
	{
		return INVOKE( getenv, ( name ) );
	}
	
	int setenv( const char* name, const char* value, int overwrite )
	{
		return INVOKE( setenv, ( name, value, overwrite ) );
	}
	
	int putenv( const char* string )
	{
		return INVOKE( putenv, ( string ) );
	}
	
	void unsetenv( const char* name )
	{
		return INVOKE( unsetenv, ( name ) );
	}
	
	int clearenv()
	{
		return INVOKE( clearenv, () );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/ioctl ¥
	
	int ioctl( int fd, unsigned long cmd, int* argp )
	{
		return INVOKE( ioctl, ( fd, cmd, argp ) );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/ptrace ¥
	
	int ptrace( int request, pid_t pid, void* addr, int data )
	{
		return INVOKE( ptrace, ( request, pid, addr, data ) );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/select ¥
	
	int select( int n, fd_set*  readfds,
	                   fd_set*  writefds,
	                   fd_set*  exceptfds, struct timeval* timeout )
	{
		return INVOKE( select, ( n, readfds, writefds, exceptfds, timeout ) );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/socket ¥
	
	int socketpair( int domain, int type, int protocol, int fds[2] )
	{
		return INVOKE( socketpair, ( domain, type, protocol, fds ) );
	}
	
	int socket( int domain, int type, int protocol )
	{
		return INVOKE( socket, ( domain, type, protocol ) );
	}
	
	int bind( int sockfd, const struct sockaddr* name, socklen_t namelen )
	{
		return INVOKE( bind, ( sockfd, name, namelen ) );
	}
	
	int listen( int sockfd, int backlog )
	{
		return INVOKE( listen, ( sockfd, backlog ) );
	}
	
	int accept( int sockfd, struct sockaddr* addr, socklen_t* addrlen )
	{
		return INVOKE( accept, ( sockfd, addr, addrlen ) );
	}
	
	int connect( int sockfd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		return INVOKE( connect, ( sockfd, serv_addr, addrlen ) );
	}
	
	ssize_t send  ( int s, const void* buf, size_t len, int flags )
	{
		return INVOKE( send, ( s, buf, len, flags ) );
	}
	
	ssize_t sendto( int s, const void* buf, size_t len, int flags, const struct sockaddr* to, socklen_t tolen )
	{
		return INVOKE( sendto, ( s, buf, len, flags, to, tolen ) );
	}
	
	ssize_t recv( int s, void* buf, size_t len, int flags )
	{
		return INVOKE( recv, ( s, buf, len, flags ) );
	}
	
	ssize_t recvfrom( int s, void* buf, size_t len, int flags, struct sockaddr* from, socklen_t* fromlen )
	{
		return INVOKE( recvfrom, ( s, buf, len, flags, from, fromlen ) );
	}
	
	int getsockname( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		return INVOKE( getsockname, ( sockfd, name, namelen ) );
	}
	
	int getpeername( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		return INVOKE( getpeername, ( sockfd, name, namelen ) );
	}
	
	int getsockopt( int s, int level, int optname, void* optval, socklen_t* optlen )
	{
		return INVOKE( getsockopt, ( s, level, optname, optval, optlen ) );
	}
	
	int setsockopt( int s, int  level, int optname, const void* optval, socklen_t optlen )
	{
		return INVOKE( setsockopt, ( s, level, optname, optval, optlen ) );
	}
	
	int shutdown( int s, int how )
	{
		return INVOKE( shutdown, ( s, how ) );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/stat ¥
	
	int chmod( const char *path, mode_t mode )
	{
		return INVOKE( chmod, ( path, mode ) );
	}
	
	int fchmod( int filedes, mode_t mode )
	{
		return INVOKE( fchmod, ( filedes, mode ) );
	}
	
	int fstat( int filedes, struct stat* buf )
	{
		return INVOKE( fstat, ( filedes, buf ) );
	}
	
	int lstat( const char* file_name, struct stat* buf )
	{
		return INVOKE( lstat, ( file_name, buf ) );
	}
	
	int stat( const char* file_name, struct stat* buf )
	{
		return INVOKE( stat, ( file_name, buf ) );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/times ¥
	
	clock_t times( struct tms* tp )
	{
		return INVOKE( times, ( tp ) );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/utsname ¥
	
	int uname( struct utsname* uts )
	{
		return INVOKE( uname, ( uts ) );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/wait ¥
	
	int gettimeofday( struct timeval* tv, struct timezone* tz )
	{
		return INVOKE( gettimeofday, ( tv, tz ) );
	}
	
	//int settimeofday(const struct timeval *tv , const struct timezone *tz);
	
	#pragma mark -
	#pragma mark ¥ sys/wait ¥
	
	pid_t waitpid( pid_t pid, int* stat_loc, int options )
	{
		return INVOKE( waitpid, ( pid, stat_loc, options ) );
	}
	
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	unsigned int alarm( unsigned int seconds )
	{
		return INVOKE( alarm, ( seconds ) );
	}
	
	int chdir( const char* path )
	{
		return INVOKE( chdir, ( path ) );
	}
	
	int close( int filedes )
	{
		return INVOKE( close, ( filedes ) );
	}
	
	int copyfile( const char* src, const char* dest )
	{
		return INVOKE( copyfile, ( src, dest ) );
	}
	
	int dup( int filedes )
	{
		return INVOKE( dup, ( filedes ) );
	}
	
	int dup2( int filedes, int filedes2 )
	{
		return INVOKE( dup2, ( filedes, filedes2 ) );
	}
	
	int execve( const char* path, const char* const* argv, const char* const* envp )
	{
		return INVOKE( execve, ( path, argv, envp ) );
	}
	
	void _exit( int status )
	{
		return INVOKE_CRITICAL( _exit, ( status ) );  // Terminates process but returns if forked
	}
	
	pid_t fork_and_exit( int status )
	{
		return INVOKE( fork_and_exit, ( status ) );
	}
	
	int SpawnVFork( void (*LongJmp)( int ) )
	{
		return INVOKE( SpawnVFork, ( LongJmp ) );
	}
	
	char* getcwd( char* buf, size_t size )
	{
		return INVOKE( getcwd, ( buf, size ) );
	}
	
	pid_t getpid()
	{
		return INVOKE_CRITICAL( getpid, () );
	}
	
	pid_t getpgid( pid_t pid )
	{
		return INVOKE( getpgid, ( pid ) );
	}
	
	pid_t getppid()
	{
		return INVOKE( getppid, () );
	}
	
	off_t lseek( int fildes, off_t offset, int whence )
	{
		return INVOKE( lseek, ( fildes, offset, whence ) );
	}
	
	int mkdir( const char* pathname, mode_t mode )
	{
		return INVOKE( mkdir, ( pathname, mode ) );
	}
	
	int rmdir( const char* pathname )
	{
		return INVOKE( rmdir, ( pathname ) );
	}
	
	int pause()
	{
		return INVOKE( pause, () );
	}
	
	/*
	int peek( int fd, const char** buffer, size_t minBytes )
	{
		return INVOKE( peek, ( fd, buffer, minBytes ) );
	}
	*/
	
	int pipe( int filedes[ 2 ] )
	{
		return INVOKE( pipe, ( filedes ) );
	}
	
	ssize_t read( int filedes, void* buf, size_t nbyte )
	{
		return INVOKE( read, ( filedes, buf, nbyte ) );
	}
	
	int readlink( const char* path, char* buf, size_t len )
	{
		return INVOKE( readlink, ( path, buf, len ) );
	}
	
	int rename( const char* src, const char* dest )
	{
		return INVOKE( rename, ( src, dest ) );
	}
	
	int setpgid( pid_t pid, pid_t pgid )
	{
		return INVOKE( setpgid, ( pid, pgid ) );
	}
	
	pid_t setsid()
	{
		return INVOKE( setsid, () );
	}
	
	unsigned int sleep( unsigned int seconds )
	{
		return INVOKE( sleep, ( seconds ) );
	}
	
	int symlink( const char* target, const char* link )
	{
		return INVOKE( symlink, ( target, link ) );
	}
	
	int truncate( const char* path, off_t length )
	{
		return INVOKE( truncate, ( path, length ) );
	}
	
	int ftruncate( int fd, off_t length )
	{
		return INVOKE( ftruncate, ( fd, length ) );
	}
	
	const char* ttyname( int filedes )
	{
		return INVOKE( ttyname, ( filedes ) );
	}
	
	int ttypair( int filedes[ 2 ] )
	{
		return INVOKE( ttypair, ( filedes ) );
	}
	
	int unlink( const char* pathname )
	{
		return INVOKE( unlink, ( pathname ) );
	}
	
	ssize_t write( int filedes, const void* buf, size_t nbyte )
	{
		if ( LOAD_SYMBOL( write ) == NULL )
		{
			// There's not much we can do.
			// write() and _exit() are currently in the same module,
			// and the chance that kill() is available is very slight.
			
			EnterComa();
		}
		
		return INVOKE_COMMON( write, ( filedes, buf, nbyte ) );
	}
	
	#pragma mark -
	#pragma mark ¥ Genie ¥
	
#if TARGET_CPU_68K
	
	void InitProc( CleanupHandler cleanup )
	{
		INVOKE( InitProc, ( cleanup ) );
	}
	
#endif
	
	OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply )
	{
		return INVOKE( AESendBlocking, ( appleEvent, reply ) );
	}
	
	FSSpec Path2FSS( const char* pathname )
	{
		FSSpec spec;	
		
		OSStatus err = INVOKE( Path2FSSpec, ( pathname, &spec ) );
		
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

