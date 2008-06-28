/*	==================
 *	FragmentInitTerm.c
 *	==================
 */

// FragmentInitTerm
#include "FragmentInitTermProcs.h"

#undef FRAGMENT_INIT_TERM_PROCS_ARRAY

extern pascal OSErr InitializeLampTool( const CFragInitBlock* );

#define FRAGMENT_INIT_TERM_PROCS_ARRAY  { { &InitializeLampTool, NULL }, FRAGMENT_INIT_TERM_PROCS }

// FragmentInitTerm
#include "FragmentInitTerm.h"

// Initialize our copy of the dispatcher's address from (dynamic) ToolScratch
extern void InitializeDispatcher();

// Initialize environ from ToolScratch
extern void InitializeEnviron();

pascal OSErr InitializeLampTool( const CFragInitBlock* )
{
	InitializeDispatcher();
	InitializeEnviron();
	
	return noErr;
}

