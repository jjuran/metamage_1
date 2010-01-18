/*	=============
 *	InitTool68K.c
 *	=============
 */

#ifdef __MC68K__

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif

extern void InitializeTool();

extern void _set_dispatcher( void* address );

// Initialize environ from ToolScratch
extern const void* InitializeEnviron();

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();

// Call main() and exit()
extern void _lamp_main( int argc, char** argv, char** envp );
extern int        main( int argc, char** argv );

extern void  exit( int );
extern void _exit( int );


#pragma force_active on

void InitializeTool()
{
	void **const toolScratch = (void**) LMGetToolScratch();
	
	_set_dispatcher( toolScratch[ 0 ] );
	
	if ( InitializeEnviron() == 0L )
	{
		_exit( 127 );
	}
		
	InitializeCallbacks();
}

void _lamp_main( int argc, char** argv, char** envp )
{
	const int status = main( argc, argv );
	
	exit( status );
}

#pragma force_active reset

#endif

