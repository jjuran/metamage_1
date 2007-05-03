/*	====================
 *	FreeTheMallocPool.cc
 *	====================
 */


#include "FreeTheMallocPool.h"

// MSL
#include <pool_alloc.h>


#pragma exceptions off


void FreeTheMallocPool()
{
	__pool_free_all();
}

