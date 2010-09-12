/*
	define_syscalls.h
	-----------------
*/

// Lamp
#include "lamp/syscalls.h"


DEFINE_SYSCALL_7F( _exit )
DEFINE_SYSCALL_7F( vfork_start )
DEFINE_SYSCALL_7F( read  )
DEFINE_SYSCALL_7F( write )
//DEFINE_SYSCALL_7F( open  )
DEFINE_SYSCALL_7F( close )
DEFINE_SYSCALL_7F( waitpid )
//DEFINE_SYSCALL_7F( _realpath )

//DEFINE_SYSCALL_7F( unlink )
DEFINE_SYSCALL_7F( execve )
DEFINE_SYSCALL_7F( chdir  )

//DEFINE_SYSCALL_7F( mknod )
//DEFINE_SYSCALL_7F( chmod )

DEFINE_SYSCALL_7F( reexec )
//DEFINE_SYSCALL_7F( getcwd )
DEFINE_SYSCALL_7F( lseek )
DEFINE_SYSCALL_7F( getpid )

DEFINE_SYSCALL_7F( ptrace )
DEFINE_SYSCALL_7F( alarm )

DEFINE_SYSCALL_7F( pause )
//DEFINE_SYSCALL_7F( utime )

//DEFINE_SYSCALL_7F( access )

DEFINE_SYSCALL_7F( sync )
DEFINE_SYSCALL_7F( kill )
//DEFINE_SYSCALL_7F( rename )
//DEFINE_SYSCALL_7F( mkdir )
//DEFINE_SYSCALL_7F( rmdir )
//DEFINE_SYSCALL_7F( dup )
//DEFINE_SYSCALL_7F( pipe )
DEFINE_SYSCALL_7F( times )

//DEFINE_SYSCALL_7F( signal )
DEFINE_SYSCALL_7F( flock )
DEFINE_SYSCALL_7F( ioctl )
DEFINE_SYSCALL_7F( fcntl )

DEFINE_SYSCALL_7F( setpgid )

DEFINE_SYSCALL_7F( uname )
DEFINE_SYSCALL_7F( umask )

//DEFINE_SYSCALL_7F( dup2 )
DEFINE_SYSCALL_7F( getppid )

DEFINE_SYSCALL_7F( setsid )
DEFINE_SYSCALL_7F( sigaction )

DEFINE_SYSCALL_7F( sigsuspend )
DEFINE_SYSCALL_7F( sigpending )

DEFINE_SYSCALL_7F( gettimeofday )

DEFINE_SYSCALL_7F( select )
//DEFINE_SYSCALL_7F( symlink )

DEFINE_SYSCALL_7F( _readlinkat )
DEFINE_SYSCALL_7F( _realpathat )

DEFINE_SYSCALL_7F( mmap )
DEFINE_SYSCALL_7F( munmap )
DEFINE_SYSCALL_7F( truncate )
DEFINE_SYSCALL_7F( ftruncate )
DEFINE_SYSCALL_7F( fchmod )

DEFINE_SYSCALL_7F( setitimer )
DEFINE_SYSCALL_7F( getitimer )
//DEFINE_SYSCALL_7F( stat )
//DEFINE_SYSCALL_7F( lstat )
DEFINE_SYSCALL_7F( fstat )

DEFINE_SYSCALL_7F( fsync )

DEFINE_SYSCALL_7F( sigprocmask )

DEFINE_SYSCALL( getpgid )
DEFINE_SYSCALL( fchdir )

DEFINE_SYSCALL( getdents )

DEFINE_SYSCALL( writev )
DEFINE_SYSCALL( getsid )
DEFINE_SYSCALL( fdatasync )

DEFINE_SYSCALL( nanosleep )

//DEFINE_SYSCALL( poll )

DEFINE_SYSCALL( pread )
DEFINE_SYSCALL( pwrite )

DEFINE_SYSCALL( openat       )
DEFINE_SYSCALL( mkdirat      )
DEFINE_SYSCALL( mknodat      )
DEFINE_SYSCALL( fchownat     )
//DEFINE_SYSCALL( futimesat    )
DEFINE_SYSCALL( fstatat      )
DEFINE_SYSCALL( unlinkat     )
DEFINE_SYSCALL( renameat     )
DEFINE_SYSCALL( linkat       )
DEFINE_SYSCALL( symlinkat    )
//DEFINE_SYSCALL( readlinkat )
DEFINE_SYSCALL( fchmodat     )
DEFINE_SYSCALL( faccessat    )

DEFINE_SYSCALL( copyfileat )
DEFINE_SYSCALL( updateat   )

DEFINE_SYSCALL( socketpair )
DEFINE_SYSCALL( socket )
DEFINE_SYSCALL( bind )
DEFINE_SYSCALL( listen )
DEFINE_SYSCALL( accept )
DEFINE_SYSCALL( connect )
DEFINE_SYSCALL( shutdown )
DEFINE_SYSCALL( getsockname )
DEFINE_SYSCALL( getpeername )
DEFINE_SYSCALL( getsockopt )
DEFINE_SYSCALL( setsockopt )
DEFINE_SYSCALL( send )
DEFINE_SYSCALL( recv )
DEFINE_SYSCALL( sendto )
DEFINE_SYSCALL( recvfrom )

DEFINE_SYSCALL( gethostbyname )

DEFINE_SYSCALL( fork_and_exit )

DEFINE_SYSCALL( pump )

DEFINE_SYSCALL( ttypair )

DEFINE_SYSCALL( copyfile )

DEFINE_SYSCALL( AESendBlocking )
DEFINE_SYSCALL( _OTInetMailExchange )

DEFINE_SYSCALL( utimensat )

DEFINE_SYSCALL( dup3 )
DEFINE_SYSCALL( pipe2 )

