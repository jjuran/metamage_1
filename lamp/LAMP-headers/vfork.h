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
	
	extern jmp_buf gKerosene_vfork_jmp_buf;
	
	int SpawnVFork();  // Only returns 0 or -1
	int Kerosene_SpawnVFork();
	
	#define Kerosene_vfork() ( Kerosene_SpawnVFork() ? -1 : setjmp(gKerosene_vfork_jmp_buf) )
	
#ifdef __cplusplus
}
#endif

#endif

