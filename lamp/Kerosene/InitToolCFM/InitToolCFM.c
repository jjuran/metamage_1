/*	=============
 *	InitToolCFM.c
 *	=============
 */

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// MSL Runtime
extern pascal OSErr __initialize( const struct CFragInitBlock* initBlock );
extern pascal void  __terminate ();

// Lamp runtime
extern pascal OSErr _initialize_lamp( const struct CFragInitBlock* initBlock );


extern void _set_dispatcher( void* address );

// Initialize environ from ToolScratch
extern const void* _initialize_environ( char **envp );

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();


// Call main() and exit()
extern void _lamp_main( int argc, char** argv, char** envp );
extern int        main( int argc, char** argv );

extern void exit( int );


#pragma export on

pascal OSErr _initialize_lamp( const struct CFragInitBlock* initBlock )
{
	void **const toolScratch = (void**) LMGetToolScratch();
	
	_set_dispatcher( toolScratch[ 0 ] );
	
	if ( _initialize_environ( (char**) toolScratch[ 1 ] ) == NULL )
	{
		return memFullErr;
	}
	
	InitializeCallbacks();
	
	return __initialize( initBlock );
}

#pragma export reset


void _lamp_main( int argc, char** argv, char** envp )
{
	const int status = main( argc, argv );
	
	exit( status );
}

