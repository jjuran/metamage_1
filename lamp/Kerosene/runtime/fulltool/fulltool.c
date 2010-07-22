/*
	fulltool.c
	----------
*/


// MSL Runtime
#ifdef __MC68K__
extern void __InitCode__();
#define INITIALIZE()  __InitCode__()
#else
extern pascal short __initialize( const void* initBlock );
#define INITIALIZE()  __initialize( 0 )  /* NULL */
#endif

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
	
	INITIALIZE();
	
	exit( main( argc, argv ) );
}

#pragma force_active reset

