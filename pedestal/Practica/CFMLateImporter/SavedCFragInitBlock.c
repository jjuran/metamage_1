/*	=====================
 *	SavedCFragInitBlock.c
 *	=====================
 */

#include "CFMLateImporter/SavedCFragInitBlock.h"

// Mac OS
#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

/*

// FragmentInitTerm
#include "FragmentInitTermProcs.h"

#undef FRAGMENT_INIT_TERM_PROCS_ARRAY

static pascal OSErr SaveInitBlock_Initialize( const CFragInitBlock* initBlock );

#define FRAGMENT_INIT_TERM_PROCS_ARRAY  { { &SaveInitBlock_Initialize, NULL } }

// FragmentInitTerm
#include "FragmentInitTerm.h"

*/


static CFragInitBlock  gSavedInitBlock;
static FSSpec          gSavedFileSpec;

const CFragInitBlock* SavedCFragInitBlock()
{
	return &gSavedInitBlock;
}


OSErr Initialize_SavedCFragInitBlock( const CFragInitBlock* initBlock )
{
	SInt32 where = initBlock->fragLocator.where;
	
	gSavedInitBlock = *initBlock;
	gSavedFileSpec  = *initBlock->fragLocator.u.onDisk.fileSpec;
	
	if ( where == kDataForkCFragLocator  ||  where == kResourceCFragLocator )
	{
		gSavedInitBlock.fragLocator.u.onDisk.fileSpec = &gSavedFileSpec;
	}
	
	return noErr;
}

