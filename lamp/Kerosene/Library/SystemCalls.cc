/*	==============
 *	SystemCalls.cc
 *	==============
 */

// Mac OS
#include <LowMem.h>

// Lamp
#include "lamp/syscalls.h"


#pragma exceptions off


#if TARGET_CPU_68K
	
	extern "C" void InitializeProcess();
	
	enum { kDispatcherAddr = (long) LMGetToolScratch() };
	
	static asm void SystemCall()
	{
		JSR			InitializeProcess	;
		
		MOVEA.L		kDispatcherAddr,A0	;  // load the dispatcher's address
										;  // arg 1:  syscall index already on stack
		JMP			(A0)				;  // jump to dispatcher -- doesn't return
		
		// Not reached
	}
	
	#define DEFINE_STUB( name ) extern "C" void name(); asm void name() { MOVE.L #__NR_##name,-(SP) ;  JMP SystemCall }
	
#endif

#if TARGET_CPU_PPC
	
	#pragma export on
		
		void    (*gDispatcher)();
		
	#pragma export reset
	
	static asm void SystemCall()
	{
		mflr	r0				// get caller's return address
		stw		r0,8(SP)		// store return address in caller's stack frame
		
		stwu	SP,-64(SP)		// allocate our own stack frame
		
		stw		RTOC,20(SP)		// save RTOC into stack
		
		stw		r3,24(SP)		// save system call arguments into stack
		stw		r4,28(SP)
		stw		r5,32(SP)
		stw		r6,36(SP)
		stw		r7,40(SP)
		stw		r8,44(SP)
		stw		r9,48(SP)
		stw		r10,52(SP)
		
		lwz		r12,gDispatcher	// load address of export
		lwz		r12,0(r12)		// load dispatcher T-vector
		lwz		r0,0(r12)		// load function address
		lwz		RTOC,4(r12)		// load foreign RTOC
		mtctr	r0				// ready, aim...
		bctrl					// branch to dispatcher (with return link)
		lwz		RTOC,20(SP)		// restore our RTOC
		
		addi	SP,SP,64		// deallocate our stack frame
		
		lwz		r0,8(SP)		// reload caller's return address
		mtlr	r0				// load it into the link register
		blr						// return
	}
	
	#define DEFINE_STUB( name ) extern "C" void name(); asm void name() { addi r11,0,__NR_##name ;  b SystemCall }
	
#endif

#if TARGET_CPU_68K
	
	DEFINE_STUB( InitProc )
	
#endif

DEFINE_STUB( _exit )
DEFINE_STUB( SpawnVFork )
DEFINE_STUB( read  )
DEFINE_STUB( write )
DEFINE_STUB( open  )
DEFINE_STUB( close )
DEFINE_STUB( waitpid )

DEFINE_STUB( unlink )
DEFINE_STUB( execve )
DEFINE_STUB( chdir  )

DEFINE_STUB( mknod )
DEFINE_STUB( chmod )

DEFINE_STUB( fchdir )
DEFINE_STUB( getcwd_k )
DEFINE_STUB( lseek )
DEFINE_STUB( getpid )

DEFINE_STUB( ptrace )
DEFINE_STUB( alarm )
DEFINE_STUB( fork_and_exit )
DEFINE_STUB( pause )

DEFINE_STUB( copyfile )

DEFINE_STUB( kill )
DEFINE_STUB( rename )
DEFINE_STUB( mkdir )
DEFINE_STUB( rmdir )
DEFINE_STUB( dup )
DEFINE_STUB( pipe )
DEFINE_STUB( times )

DEFINE_STUB( signal )
DEFINE_STUB( flock )
DEFINE_STUB( ioctl )
DEFINE_STUB( fcntl )
DEFINE_STUB( getpgid )
DEFINE_STUB( setpgid )
DEFINE_STUB( getsid )
DEFINE_STUB( uname )

DEFINE_STUB( dup2 )
DEFINE_STUB( getppid )

DEFINE_STUB( setsid )

DEFINE_STUB( sigaction )

DEFINE_STUB( sigsuspend )
DEFINE_STUB( sigpending )

DEFINE_STUB( gettimeofday )

DEFINE_STUB( select )
DEFINE_STUB( symlink )

DEFINE_STUB( readlink_k )

DEFINE_STUB( truncate )
DEFINE_STUB( ftruncate )
DEFINE_STUB( fchmod )

DEFINE_STUB( stat )
DEFINE_STUB( lstat )
DEFINE_STUB( fstat )

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

DEFINE_STUB( getenv )
DEFINE_STUB( setenv )
DEFINE_STUB( unsetenv )
DEFINE_STUB( putenv )
DEFINE_STUB( clearenv )

DEFINE_STUB( opendir )
DEFINE_STUB( closedir )
DEFINE_STUB( readdir )
DEFINE_STUB( rewinddir )
DEFINE_STUB( seekdir )
DEFINE_STUB( telldir )
DEFINE_STUB( dirfd )

DEFINE_STUB( ttypair )

DEFINE_STUB( gethostbyname )
DEFINE_STUB( Path2FSSpec )
DEFINE_STUB( AESendBlocking )
DEFINE_STUB( OTInetMailExchange_k )

DEFINE_STUB( sleep )

