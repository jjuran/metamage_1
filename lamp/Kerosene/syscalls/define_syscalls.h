/*
	define_syscalls.h
	-----------------
*/

// Lamp
#include "lamp/syscalls.h"


DEFINE_STUB_7F( _exit )
DEFINE_STUB_7F( vfork_start )
DEFINE_STUB_7F( read  )
DEFINE_STUB_7F( write )
//DEFINE_STUB_7F( open  )
DEFINE_STUB_7F( close )
DEFINE_STUB_7F( waitpid )
//DEFINE_STUB_7F( _realpath )

//DEFINE_STUB_7F( unlink )
DEFINE_STUB_7F( execve )
DEFINE_STUB_7F( chdir  )

//DEFINE_STUB_7F( mknod )
//DEFINE_STUB_7F( chmod )

DEFINE_STUB_7F( reexec )
//DEFINE_STUB_7F( getcwd )
DEFINE_STUB_7F( lseek )
DEFINE_STUB_7F( getpid )

DEFINE_STUB_7F( ptrace )
DEFINE_STUB_7F( alarm )

DEFINE_STUB_7F( pause )
//DEFINE_STUB_7F( utime )

//DEFINE_STUB_7F( access )

DEFINE_STUB_7F( sync )
DEFINE_STUB_7F( kill )
//DEFINE_STUB_7F( rename )
//DEFINE_STUB_7F( mkdir )
//DEFINE_STUB_7F( rmdir )
//DEFINE_STUB_7F( dup )
//DEFINE_STUB_7F( pipe )
DEFINE_STUB_7F( times )

//DEFINE_STUB_7F( signal )
DEFINE_STUB_7F( flock )
DEFINE_STUB_7F( ioctl )
DEFINE_STUB_7F( fcntl )

DEFINE_STUB_7F( setpgid )

DEFINE_STUB_7F( uname )
DEFINE_STUB_7F( umask )

//DEFINE_STUB_7F( dup2 )
DEFINE_STUB_7F( getppid )

DEFINE_STUB_7F( setsid )
DEFINE_STUB_7F( sigaction )

DEFINE_STUB_7F( sigsuspend )
DEFINE_STUB_7F( sigpending )

DEFINE_STUB_7F( gettimeofday )

DEFINE_STUB_7F( select )
//DEFINE_STUB_7F( symlink )

DEFINE_STUB_7F( _readlinkat )
DEFINE_STUB_7F( _realpathat )

DEFINE_STUB_7F( mmap )
DEFINE_STUB_7F( munmap )
DEFINE_STUB_7F( truncate )
DEFINE_STUB_7F( ftruncate )
DEFINE_STUB_7F( fchmod )

DEFINE_STUB_7F( setitimer )
DEFINE_STUB_7F( getitimer )
//DEFINE_STUB_7F( stat )
//DEFINE_STUB_7F( lstat )
DEFINE_STUB_7F( fstat )

DEFINE_STUB_7F( fsync )

DEFINE_STUB_7F( sigprocmask )

DEFINE_STUB_FF( getpgid )
DEFINE_STUB_FF( fchdir )

DEFINE_STUB_FF( getdents )

DEFINE_STUB_FF( writev )
DEFINE_STUB_FF( getsid )
DEFINE_STUB_FF( fdatasync )

DEFINE_STUB_FF( nanosleep )

//DEFINE_STUB_FF( poll )

DEFINE_STUB_FF( pread )
DEFINE_STUB_FF( pwrite )

DEFINE_STUB_FF( openat       )
DEFINE_STUB_FF( mkdirat      )
DEFINE_STUB_FF( mknodat      )
DEFINE_STUB_FF( fchownat     )
//DEFINE_STUB_FF( futimesat    )
DEFINE_STUB_FF( fstatat      )
DEFINE_STUB_FF( unlinkat     )
DEFINE_STUB_FF( renameat     )
DEFINE_STUB_FF( linkat       )
DEFINE_STUB_FF( symlinkat    )
//DEFINE_STUB_FF( readlinkat )
DEFINE_STUB_FF( fchmodat     )
DEFINE_STUB_FF( faccessat    )

DEFINE_STUB_FF( copyfileat )
DEFINE_STUB_FF( updateat   )

DEFINE_STUB( socketpair )
DEFINE_STUB( socket )
DEFINE_STUB( bind )
DEFINE_STUB( listen )
DEFINE_STUB( accept )
DEFINE_STUB( connect )
DEFINE_STUB( shutdown )
DEFINE_STUB( getsockname )
DEFINE_STUB( getpeername )
DEFINE_STUB( getsockopt )
DEFINE_STUB( setsockopt )
DEFINE_STUB( send )
DEFINE_STUB( recv )
DEFINE_STUB( sendto )
DEFINE_STUB( recvfrom )

DEFINE_STUB( gethostbyname )

DEFINE_STUB( fork_and_exit )

DEFINE_STUB( pump )

DEFINE_STUB( ttypair )

DEFINE_STUB( copyfile )

DEFINE_STUB( AESendBlocking )
DEFINE_STUB( _OTInetMailExchange )

DEFINE_STUB( utimensat )

DEFINE_STUB( dup3 )
DEFINE_STUB( pipe2 )

