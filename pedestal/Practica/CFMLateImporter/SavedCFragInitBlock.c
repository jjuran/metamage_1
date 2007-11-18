/*	=====================
 *	SavedCFragInitBlock.c
 *	=====================
 */

#include "SavedCFragInitBlock.h"

// Mac OS Universal Interfaces
#include <CodeFragments.h>

/*

// FragmentInitTerm
#include "FragmentInitTermProcs.h"

#undef FRAGMENT_INIT_TERM_PROCS_ARRAY

static pascal OSErr SaveInitBlock_Initialize( const CFragInitBlock* initBlock );

#define FRAGMENT_INIT_TERM_PROCS_ARRAY  { { &SaveInitBlock_Initialize, NULL } }

// FragmentInitTerm
#include "FragmentInitTerm.h"

*/

pascal OSErr Initialize_SavedCFragInitBlock( const CFragInitBlock* initBlock );


static CFragInitBlock  gSavedInitBlock;
static FSSpec          gSavedFileSpec;

const CFragInitBlock* SavedCFragInitBlock()
{
	return &gSavedInitBlock;
}


pascal OSErr Initialize_SavedCFragInitBlock( const CFragInitBlock* initBlock )
{
	SInt32 where = initBlock->fragLocator.where;
	
	gSavedInitBlock = *initBlock;
	gSavedFileSpec  = *initBlock->fragLocator.u.onDisk.fileSpec;
	
	BlockMoveData( initBlock,                                &gSavedInitBlock, sizeof gSavedInitBlock );
	BlockMoveData( initBlock->fragLocator.u.onDisk.fileSpec, &gSavedFileSpec,  sizeof gSavedFileSpec );
	
	if ( where == kDataForkCFragLocator  ||  where == kResourceCFragLocator )
	{
		gSavedInitBlock.fragLocator.u.onDisk.fileSpec = &gSavedFileSpec;
	}
	
	return noErr;
}

