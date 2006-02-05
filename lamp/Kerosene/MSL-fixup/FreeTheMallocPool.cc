/*	====================
 *	FreeTheMallocPool.cc
 *	====================
 */


#include "FreeTheMallocPool.h"

// MSL
#include <pool_alloc.h>


void FreeTheMallocPool()
{
	__pool_free_all();
}

