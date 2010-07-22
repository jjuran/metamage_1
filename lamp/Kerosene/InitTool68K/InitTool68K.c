/*	=============
 *	InitTool68K.c
 *	=============
 */

#ifdef __MC68K__

extern void __InitCode__();

extern void _set_dispatcher( void* address );

extern char** environ;

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();

// Call main() and exit()
extern void _lamp_main( int argc, char** argv, char** envp, void* dispatcher );
extern int        main( int argc, char** argv );

extern void exit( int );


#pragma force_active on

void _lamp_main( int argc, char** argv, char** envp, void* dispatcher )
{
	_set_dispatcher( dispatcher );
	
	environ = envp;
	
	InitializeCallbacks();
	
	__InitCode__();
	
	exit( main( argc, argv ) );
}

#pragma force_active reset

#endif

