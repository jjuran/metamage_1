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
	
	int SpawnVFork( void (*LongJmp)( int ) );  // Only returns 0 or -1
	int Kerosene_SpawnVFork();
	
	jmp_buf* NewJmpBuf();
	
	#define Kerosene_vfork() ( Kerosene_SpawnVFork() ? -1 : setjmp( *NewJmpBuf() ) )
	
#ifdef __cplusplus
}
#endif

#endif

