/*
	syscalls.c
	----------
	
	Joshua Juran
*/

// Lamp
#include "lamp/syscalls.h"


static void* global_dispatcher;

extern void _set_dispatcher( void* address );

void _set_dispatcher( void* address )
{
	global_dispatcher = address;
}

extern int syscall( int number, ... );

#ifdef __MC68K__
	
	static void SystemCall();
	
	asm int syscall( int number, ... )
	{
		MOVE.L  4(SP), D0    // copy system call number to d0
		MOVE.L  (SP)+, (SP)  // overwrite it with the return address, and pop
		
		TRAP  #0
	}
	
	#define DEFINE_STUB_7F( name )    \
		extern void name();           \
		asm void name()               \
		{                             \
			MOVEQ #__NR_##name,D0  ;  \
			TRAP  #0               ;  \
		}
	
	#define DEFINE_STUB_FF( name )    \
			DEFINE_STUB( name )
	
	#define DEFINE_STUB( name )       \
		extern void name();           \
		asm void name()               \
		{                             \
			MOVE.W #__NR_##name,D0 ;  \
			TRAP  #0               ;  \
		}
	
#endif

#ifdef __POWERPC__
	
	extern void __ptr_glue();
	
	static asm void SystemCall()
	{
		nofralloc
		
		mflr	r0						// get caller's return address
		stw		r0,8(SP)				// store it in caller's stack frame
		
		stwu	SP,-64(SP)				// allocate our own stack frame
		
		lwz		r12,global_dispatcher	// load dispatcher T-vector
		
		bl		__ptr_glue				// cross-TOC call
		nop								// synchronize pipeline
		lwz		RTOC,20(SP)				// restore our RTOC
		
		addi	SP,SP,64				// deallocate our stack frame
		
		lwz		r0,8(SP)				// reload caller's return address
		mtlr	r0						// load it into the link register
		blr								// return
	}
	
	asm int syscall( int number, ... )
	{
		nofralloc
		
		// copy system call number to r11
		mr  r11,r3
		
		// bump up to six arguments into the correct registers
		mr  r3,r4
		mr  r4,r5
		mr  r5,r6
		mr  r6,r7
		mr  r7,r8
		mr  r8,r9
		
		b   SystemCall
	}
	
	#define DEFINE_STUB( name )   \
		extern void name();       \
		asm void name()           \
		{                         \
			li r11,__NR_##name ;  \
			b SystemCall       ;  \
		}
	
	#define DEFINE_STUB_7F( name )  DEFINE_STUB( name )
	#define DEFINE_STUB_FF( name )  DEFINE_STUB( name )
	
#endif


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

