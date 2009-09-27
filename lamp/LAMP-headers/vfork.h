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
	
	int _vfork_start( jmp_buf* buffer );
	
	inline pid_t vfork()
	{
		jmp_buf *const temp_jmp_buf = (jmp_buf*) alloca( sizeof (jmp_buf) );
		
		return (pid_t) _vfork_start( temp_jmp_buf ) ? -1
		                                            : setjmp( *temp_jmp_buf );
	}
	
#ifdef __cplusplus
}
#endif

#endif

