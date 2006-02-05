/*	==============
 *	KernelStubs.cc
 *	==============
 */

// POSIX
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
#include "KernelGlue.hh"


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
	
	//#pragma mark -
	#pragma mark ¥ fcntl ¥
	
	int fcntl( int fd, int cmd, int param )
	{
		static Stub3< int, int, int, int > stub( "fcntl" );
		
		return stub( fd, cmd, param );
	}
	
	int open( const char* path, int oflags, mode_t mode )
	{
		static Stub2< int, const char*, int > stub( "open" );
		
		return stub( path, oflags );
	}
	
	#pragma mark -
	#pragma mark ¥ signal ¥
	
	int kill( pid_t pid, int sig )
	{
		static Stub2< int, pid_t, int > stub( "kill" );
		
		return stub( pid, sig );
	}
	
	int sigaction( int signum, const struct sigaction* act, struct sigaction* oldact )
	{
		static Stub3< int, int, const struct sigaction*, struct sigaction* > stub( "sigaction" );
		
		return stub( signum, act, oldact );
	}
	
	int sigprocmask( int how, const sigset_t* set, sigset_t* oldset )
	{
		static Stub3< int, int, const sigset_t*, sigset_t* > stub( "sigprocmask" );
		
		return stub( how, set, oldset );
	}
	
	int sigpending( sigset_t* set )
	{
		static Stub1< int, sigset_t* > stub( "sigpending" );
		
		return stub( set );
	}
	
	int sigsuspend( const sigset_t* mask )
	{
		static Stub1< int, const sigset_t* > stub( "sigsuspend" );
		
		return stub( mask );
	}
	
	__sig_handler signal( int sig, __sig_handler func )
	{
		static Stub2< __sig_handler, int, __sig_handler > stub( "signal" );
		
		return stub( sig, func );
	}
	
	#pragma mark -
	#pragma mark ¥ stdlib ¥
	
	char* getenv( const char* name )
	{
		static Stub1< char*, const char* > stub( "getenv" );
		
		return stub( name );
	}
	
	extern "C" char* GetEnv( const char* name );
	char* GetEnv( const char* name )
	{
		static Stub1< char*, const char* > stub( "getenv" );
		
		return stub( name );
	}
	
	int setenv( const char* name, const char* value, int overwrite )
	{
		static Stub3< int, const char*, const char*, int > stub( "setenv" );
		
		return stub( name, value, overwrite );
	}
	
	int putenv( const char* string )
	{
		static Stub1< int, const char* > stub( "putenv" );
		
		return stub( string );
	}
	
	void unsetenv( const char* name )
	{
		static Stub1< void, const char* > stub( "unsetenv" );
		
		return stub( name );
	}
	
	int clearenv()
	{
		static Stub0< int > stub( "clearenv" );
		
		return stub();
	}
	
	#pragma mark -
	#pragma mark ¥ sys/select ¥
	
	int select( int n, fd_set*  readfds,
	                   fd_set*  writefds,
	                   fd_set*  exceptfds, struct timeval* timeout )
	{
		static Stub5< int, int, fd_set*, fd_set*, fd_set*, struct timeval* > stub( "select" );
		
		return stub( n, readfds, writefds, exceptfds, timeout );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/socket ¥
	
	int socket( int domain, int type, int protocol )
	{
		static Stub3< int, int, int, int > stub( "socket" );
		
		return stub( domain, type, protocol );
	}
	
	int bind( int sockfd, const struct sockaddr* name, socklen_t namelen )
	{
		static Stub3< int, int, const struct sockaddr*, socklen_t > stub( "bind" );
		
		return stub( sockfd, name, namelen );
	}
	
	int listen( int sockfd, int backlog )
	{
		static Stub2< int, int, int > stub( "listen" );
		
		return stub( sockfd, backlog );
	}
	
	int accept( int sockfd, struct sockaddr* addr, socklen_t* addrlen )
	{
		static Stub3< int, int, struct sockaddr*, socklen_t* > stub( "accept" );
		
		return stub( sockfd, addr, addrlen );
	}
	
	int connect( int sockfd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		static Stub3< int, int, const struct sockaddr*, socklen_t > stub( "connect" );
		
		return stub( sockfd, serv_addr, addrlen );
	}
	
	ssize_t send  ( int s, const void* buf, size_t len, int flags )
	{
		static Stub4< ssize_t, int, const void*, size_t, int > stub( "send" );
		
		return stub( s, buf, len, flags );
	}
	
	ssize_t sendto( int s, const void* buf, size_t len, int flags, const struct sockaddr* to, socklen_t tolen )
	{
		static Stub6< ssize_t, int, const void*, size_t, int, const struct sockaddr*, socklen_t > stub( "sendto" );
		
		return stub( s, buf, len, flags, to, tolen );
	}
	
	ssize_t recv( int s, void* buf, size_t len, int flags )
	{
		static Stub4< ssize_t, int, void*, size_t, int > stub( "recv" );
		
		return stub( s, buf, len, flags );
	}
	
	ssize_t recvfrom( int s, void* buf, size_t len, int flags, struct sockaddr* from, socklen_t* fromlen )
	{
		static Stub6< ssize_t, int, void*, size_t, int, struct sockaddr*, socklen_t* > stub( "recv" );
		
		return stub( s, buf, len, flags, from, fromlen );
	}
	
	int getsockname( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		static Stub3< int, int, struct sockaddr*, socklen_t* > stub( "getsockname" );
		
		return stub( sockfd, name, namelen );
	}
	
	int getpeername( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		static Stub3< int, int, struct sockaddr*, socklen_t* > stub( "getpeername" );
		
		return stub( sockfd, name, namelen );
	}
	
	int getsockopt( int s, int level, int optname, void* optval, socklen_t* optlen )
	{
		static Stub5< int, int, int, int, void*, socklen_t* > stub( "getsockopt" );
		
		return stub( s, level, optname, optval, optlen );
	}
	
	int setsockopt( int s, int  level, int optname, const void* optval, socklen_t optlen )
	{
		static Stub5< int, int, int, int, const void*, socklen_t > stub( "setsockopt" );
		
		return stub( s, level, optname, optval, optlen );
	}
	
	int shutdown( int s, int how )
	{
		static Stub2< int, int, int > stub( "shutdown" );
		
		return stub( s, how );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/stat ¥
	
	int chmod( const char *path, mode_t mode )
	{
		static Stub2< int, const char*, mode_t > stub( "chmod" );
		
		return stub( path, mode );
	}
	
	int fchmod( int filedes, mode_t mode )
	{
		static Stub2< int, int, mode_t > stub( "fchmod" );
		
		return stub( filedes, mode );
	}
	
	int fstat( int filedes, struct stat* buf )
	{
		static Stub2< int, int, struct stat* > stub( "fstat" );
		
		return stub( filedes, buf );
	}
	
	int lstat( const char* file_name, struct stat* buf)
	{
		static Stub2< int, const char*, struct stat* > stub( "lstat" );
		
		return stub( file_name, buf );
	}
	
	int stat( const char* file_name, struct stat* buf)
	{
		static Stub2< int, const char*, struct stat* > stub( "stat" );
		
		return stub( file_name, buf );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/wait ¥
	
	int gettimeofday( struct timeval* tv, struct timezone* tz )
	{
		static Stub2< int, struct timeval*, struct timezone* > stub( "gettimeofday" );
		
		return stub( tv, tz );
	}
	
	//int settimeofday(const struct timeval *tv , const struct timezone *tz);
	
	#pragma mark -
	#pragma mark ¥ sys/wait ¥
	
	pid_t waitpid( pid_t pid, int* stat_loc, int options )
	{
		static Stub3< pid_t, pid_t, int*, int > stub( "waitpid" );
		
		return stub( pid, stat_loc, options );
	}
	
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	int chdir( const char* path )
	{
		static Stub1< int, const char* > stub( "chdir" );
		
		return stub( path );
	}
	
	int close( int filedes )
	{
		static Stub1< int, int > stub( "close" );
		
		return stub( filedes );
	}
	
	int copyfile( const char* src, const char* dest )
	{
		static Stub2< int, const char*, const char* > stub( "copyfile" );
		
		return stub( src, dest );
	}
	
	int dup( int filedes )
	{
		static Stub1< int, int > stub( "dup" );
		
		return stub( filedes );
	}
	
	int dup2( int filedes, int filedes2 )
	{
		static Stub2< int, int, int > stub( "dup2" );
		
		return stub( filedes, filedes2 );
	}
	
	int execve_Stub( const char* path, const char* const argv[], const char* const* envp )
	{
		static Stub3< int, const char*, const char* const*, const char* const* > stub( "execve" );
		
		return stub( path, argv, envp );
	}
	
	void _exit_Stub( int status )
	{
		static Stub1< void, int > stub( "_exit" );
		
		stub( status );  // Terminates process but doesn't kill thread (it returns)
	}
	
	int SpawnVFork()
	{
		static Stub0< int > stub( "SpawnVFork" );
		
		return stub();
	}
	
	char* getcwd( char* buf, size_t size )
	{
		static Stub2< char*, char*, size_t > stub( "getcwd" );
		
		return stub( buf, size );
	}
	
	pid_t getpid()
	{
		static Stub0< pid_t > stub( "getpid" );
		
		return stub();
	}
	
	pid_t getpgid( pid_t pid )
	{
		static Stub1< pid_t, pid_t > stub( "getpgid" );
		
		return stub( pid );
	}
	
	pid_t getppid()
	{
		static Stub0< pid_t > stub( "getppid" );
		
		return stub();
	}
	
	off_t lseek( int fildes, off_t offset, int whence )
	{
		static Stub3< off_t, int, off_t, int > stub( "lseek" );
		
		return stub( fildes, offset, whence );
	}
	
	int mkdir( const char* pathname, mode_t mode )
	{
		static Stub2< int, const char*, mode_t > stub( "mkdir" );
		
		return stub( pathname, mode );
	}
	
	int rmdir( const char* pathname )
	{
		static Stub1< int, const char* > stub( "rmdir" );
		
		return stub( pathname );
	}
	
	int pause()
	{
		static Stub0< int > stub( "pause" );
		
		return stub();
	}
	
	int peek( int fd, const char** buffer, size_t minBytes )
	{
		static Stub3< int, int, const char**, size_t > stub( "peek" );
		
		return stub( fd, buffer, minBytes );
	}
	
	int pipe( int filedes[ 2 ] )
	{
		static Stub1< int, int* > stub( "pipe" );
		
		return stub( filedes );
	}
	
	ssize_t read( int filedes, void* buf, size_t nbyte )
	{
		static Stub3< ssize_t, int, void*, size_t > stub  ( "read" );
		
		return stub( filedes, buf, nbyte );
	}
	
	int rename( const char* src, const char* dest )
	{
		static Stub2< int, const char*, const char* > stub( "rename" );
		
		return stub( src, dest );
	}
	
	int setctty( int filedes )
	{
		static Stub1< int, int > stub( "setctty" );
		
		return stub( filedes );
	}
	
	int setpgid( pid_t pid, pid_t pgid )
	{
		static Stub2< int, pid_t, pid_t > stub( "setpgid" );
		
		return stub( pid, pgid );
	}
	
	pid_t setsid()
	{
		static Stub0< pid_t > stub( "setsid" );
		
		return stub();
	}
	
	unsigned int sleep( unsigned int seconds )
	{
		static Stub1< unsigned int, unsigned int > stub( "sleep" );
		
		return stub( seconds );
	}
	
	const char* ttyname( int filedes )
	{
		static Stub1< const char*, int > stub( "ttyname" );
		
		return stub( filedes );
	}
	
	int ttypair( int filedes[ 2 ] )
	{
		static Stub1< int, int* > stub( "ttypair" );
		
		return stub( filedes );
	}
	
	int unlink( const char* pathname )
	{
		static Stub1< int, const char* > stub( "unlink" );
		
		return stub( pathname );
	}
	
	ssize_t write( int filedes, const void* buf, size_t nbyte )
	{
		static Stub3< ssize_t, int, const void*, size_t > stub( "write"  );
		
		return stub( filedes, buf, nbyte );
	}
	
	#pragma mark -
	#pragma mark ¥ Genie ¥
	
	int* ErrnoPtr()
	{
		static Stub0< int* > stub( "ErrnoPtr" );
		
		int* errnoPtr = stub();
		
		//ASSERT( errnoPtr != NULL );
		
		return errnoPtr;
	}
	
	char*** EnvironPtr()
	{
		static Stub0< char*** > stub( "EnvironPtr" );
		
		char*** environPtr = stub();
		
		//ASSERT( environPtr != NULL );
		
		return environPtr;
	}
	
	OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply )
	{
		static Stub2< OSStatus, const AppleEvent*, AppleEvent* > stub( "AESendBlocking" );
		
		return stub( appleEvent, reply );
	}
	
	InetSvcRef InternetServices()
	{
		static Stub0< InetSvcRef > stub( "InternetServices" );
		
		return stub();
	}
	
	void PrintPS()
	{
		static Stub0< void > stub( "PrintPS" );
		
		stub();
	}
	
	FSSpec Path2FSS( const char* pathname )
	{
		static Stub2< OSStatus, const char*, FSSpec* > stub( "Path2FSSpec" );
	
		FSSpec spec;	
		//Nitrogen::ThrowOSStatus( stub( pathname, &spec ) );
		
		OSStatus err = stub( pathname, &spec );
		
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

