/*	=============
 *	InitTool68K.c
 *	=============
 */

#ifdef __MC68K__

extern void InitializeTool( void* return_address, int argc, char** argv, char** envp, void* dispatcher );

extern void _set_dispatcher( void* address );

// Initialize environ from ToolScratch
extern const void* _initialize_environ( char **envp );

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
	
	_initialize_environ( envp );
	
	InitializeCallbacks();
}

void _lamp_main( int argc, char** argv, char** envp )
{
	const int status = main( argc, argv );
	
	exit( status );
}

#pragma force_active reset

#endif

