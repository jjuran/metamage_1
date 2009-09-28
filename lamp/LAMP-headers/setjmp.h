#ifndef	_SETJMP_H_
#define	_SETJMP_H_

#include <CWANSIIncludes/setjmp.h>

#include <sys/signal.h>


__BEGIN_DECLS

struct __sigjmp_buf_struct
{
	jmp_buf   jumpbuf;
	sigset_t  sigmask;
#if defined( __POWERPC__ ) && defined( __MWERKS__ )
	void*     backlink;  // save in case alloc() clobbers the previous SP
#endif
};

typedef struct __sigjmp_buf_struct sigjmp_buf[1];

struct __sigjmp_buf_struct* __savemasktoenv( sigjmp_buf env, int savemask );

void siglongjmp( sigjmp_buf env, int val );

__END_DECLS

#define sigsetjmp( env, savemask )  setjmp( __savemasktoenv( env, savemask )->jumpbuf )

#endif /* _SETJMP_H_ */

