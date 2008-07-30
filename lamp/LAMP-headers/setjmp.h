#ifndef	_SETJMP_H_
#define	_SETJMP_H_

#include <CWANSIIncludes/setjmp.h>

#include <sys/signal.h>


__BEGIN_DECLS

struct __sigjmp_buf_struct
{
	jmp_buf   jumpbuf;
	sigset_t  sigmask;
};

typedef __sigjmp_buf_struct sigjmp_buf[1];

__sigjmp_buf_struct* __savemasktoenv( sigjmp_buf env, int savemask );

void siglongjmp( sigjmp_buf env, int val );

__END_DECLS

#define sigsetjmp( env, savemask )  setjmp( __savemasktoenv( env, savemask )->jumpbuf )

#endif /* _SETJMP_H_ */

