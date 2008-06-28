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

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();


pascal OSErr InitializeLampTool( const CFragInitBlock* )
{
	InitializeDispatcher();
	InitializeEnviron();
	InitializeCallbacks();
	
	return noErr;
}

