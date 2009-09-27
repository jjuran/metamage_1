/*	=======
 *	vfork.h
 *	=======
 */

#ifndef KEROSENE_VFORK_H
#define KEROSENE_VFORK_H

// Standard C
#include <setjmp.h>
#include <stdlib.h>

// POSIX
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	// Rename _vfork_start() to ensure only recompiled sources link successfully.
	// _vfork_start_1 marks the switch from setjmp() to sigsetjmp().
	#define _vfork_start  _vfork_start_1
	
	int _vfork_start( sigjmp_buf* buffer );
	
	inline pid_t vfork()
	{
		sigjmp_buf *const jmpbuf = (sigjmp_buf*) alloca( sizeof (sigjmp_buf) );
		
		return (pid_t) _vfork_start( jmpbuf ) ? -1
		                                      : sigsetjmp( *jmpbuf, 1 );
	}
	
#ifdef __cplusplus
}
#endif

#endif

