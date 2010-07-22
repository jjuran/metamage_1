/*	=============
 *	InitTool68K.c
 *	=============
 */

#ifdef __MC68K__

extern void InitializeTool( void* return_address, int argc, char** argv, char** envp, void* dispatcher );

extern void _set_dispatcher( void* address );

extern char** environ;

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();

// Call main() and exit()
extern void _lamp_main( int argc, char** argv, char** envp );
extern int        main( int argc, char** argv );

extern void exit( int );


#pragma force_active on

void InitializeTool( void* return_address, int argc, char** argv, char** envp, void* dispatcher )
{
	_set_dispatcher( dispatcher );
	
	environ = envp;
	
	InitializeCallbacks();
}

void _lamp_main( int argc, char** argv, char** envp )
{
	exit( main( argc, argv ) );
}

#pragma force_active reset

#endif

