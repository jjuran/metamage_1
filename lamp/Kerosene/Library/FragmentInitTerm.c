/*	==================
 *	FragmentInitTerm.c
 *	==================
 */

// FragmentInitTerm
#include "FragmentInitTermProcs.h"

#undef FRAGMENT_INIT_TERM_PROCS_ARRAY

// Initialize our copy of the dispatcher's address from (dynamic) ToolScratch
extern pascal OSErr InitializeDispatcher( const CFragInitBlock* );
	
#define FRAGMENT_INIT_TERM_PROCS_ARRAY  { { &InitializeDispatcher, NULL }, FRAGMENT_INIT_TERM_PROCS }

// FragmentInitTerm
#include "FragmentInitTerm.h"

