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
	
	static jmp_buf* _kerosene_temp_jmp_buf;
	
	int _vfork_start( jmp_buf* buffer );
	
	#ifdef __cplusplus
		
		inline pid_t vfork()
		{
			jmp_buf* temp_jmp_buf;
			
			return _vfork_start( temp_jmp_buf
			                     = (jmp_buf*) alloca( sizeof (jmp_buf) ) )
			       ? pid_t( -1 )
			       : pid_t( setjmp( *temp_jmp_buf ) );
		}
		
	#else
		
		#define vfork()                                                   \
		        (_vfork_start( _kerosene_temp_jmp_buf                      \
		                       = (jmp_buf*) alloca( sizeof (jmp_buf) ) )  \
		         ? (pid_t) -1                                             \
		         : (pid_t) setjmp( *_kerosene_temp_jmp_buf ) )
		
	#endif
	
#ifdef __cplusplus
}
#endif

#endif

