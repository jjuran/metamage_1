/*	=============
 *	InitToolCFM.c
 *	=============
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
extern const void* InitializeEnviron();

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();

// Call main() and exit()
extern void __lamp_main( int argc, char** argv, char** envp );
extern int         main( int argc, char** argv, char** envp );

extern void exit( int );


pascal OSErr InitializeLampTool( const CFragInitBlock* )
{
	InitializeDispatcher();
	
	if ( InitializeEnviron() == NULL )
	{
		return memFullErr;
	}
	
	InitializeCallbacks();
	
	return noErr;
}

void __lamp_main( int argc, char** argv, char** envp )
{
	const int status = main( argc, argv, envp );
	
	exit( status );
}

