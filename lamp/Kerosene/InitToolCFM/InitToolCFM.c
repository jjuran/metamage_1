/*	=============
 *	InitToolCFM.c
 *	=============
 */

// Mac OS
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


// Initialize our copy of the dispatcher's address from (dynamic) ToolScratch
extern void InitializeDispatcher();

// Initialize environ from ToolScratch
extern const void* InitializeEnviron();

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();


// Call main() and exit()
extern void __lamp_main( int argc, char** argv, char** envp );
extern int         main( int argc, char** argv );

extern void exit( int );


#pragma export on

pascal OSErr _initialize_lamp( const struct CFragInitBlock* initBlock )
{
	InitializeDispatcher();
	
	if ( InitializeEnviron() == NULL )
	{
		return memFullErr;
	}
	
	InitializeCallbacks();
	
	return __initialize( initBlock );
}

#pragma export reset


void __lamp_main( int argc, char** argv, char** envp )
{
	const int status = main( argc, argv );
	
	exit( status );
}

