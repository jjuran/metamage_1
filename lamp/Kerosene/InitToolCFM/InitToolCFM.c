/*	=============
 *	InitToolCFM.c
 *	=============
 */

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// MSL Runtime
extern pascal OSErr __initialize( const struct CFragInitBlock* initBlock );
extern pascal void  __terminate ();


extern void _set_dispatcher( void* address );

// Initialize environ from envp
extern const void* _initialize_environ( char **envp );

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();


// Call main() and exit()
extern void _lamp_main( int argc, char** argv, char** envp, void* dispatcher );
extern int        main( int argc, char** argv );

extern void exit( int );


void _lamp_main( int argc, char** argv, char** envp, void* dispatcher )
{
	_set_dispatcher( dispatcher );
	
	_initialize_environ( envp );
	
	InitializeCallbacks();
	
	__initialize( NULL );
	
	exit( main( argc, argv ) );
}

