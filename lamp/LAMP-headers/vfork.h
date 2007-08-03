/*	=======
 *	vfork.h
 *	=======
 */

#ifndef KEROSENE_VFORK_H
#define KEROSENE_VFORK_H

// Standard C
#include <setjmp.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	void Kerosene_LongJmp( int result );
	
	int SpawnVFork( void (*LongJmp)( int ) );  // Only returns 0 or -1
	
	jmp_buf* NewJmpBuf();
	
	#define Kerosene_vfork() ( SpawnVFork( Kerosene_LongJmp ) ? -1 : setjmp( *NewJmpBuf() ) )
	
#ifdef __cplusplus
}
#endif

#endif

