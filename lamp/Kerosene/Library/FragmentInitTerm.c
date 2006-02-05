/*	==================
 *	FragmentInitTerm.c
 *	==================
 */

// MSL-fixup
#include "FreeTheMallocPool.h"

// FragmentInitTerm
#include "FragmentInitTermProcs.h"

#undef FRAGMENT_INIT_TERM_PROCS_ARRAY

	// Cleanup malloc pool
	static pascal void  MallocPool_Terminate();
	
#define FRAGMENT_INIT_TERM_PROCS_ARRAY  { { NULL, &MallocPool_Terminate }, FRAGMENT_INIT_TERM_PROCS }

// FragmentInitTerm
#include "FragmentInitTerm.h"


pascal void  MallocPool_Terminate()
{
	FreeTheMallocPool();
}

