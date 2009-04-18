/*	=============
 *	InitTool68K.c
 *	=============
 */

#ifdef __MC68K__

extern void InitializeTool();

// Initialize our copy of the dispatcher's address from (dynamic) ToolScratch
extern void InitializeDispatcher();

// Initialize environ from ToolScratch
extern const void* InitializeEnviron();

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();

// Call main() and exit()
extern void __lamp_main( int argc, char** argv, char** envp );
extern int         main( int argc, char** argv, char** envp );

extern void  exit( int );
extern void _exit( int );


#pragma force_active on

void InitializeTool()
{
	InitializeDispatcher();
	
	if ( InitializeEnviron() == 0L )
	{
		_exit( 127 );
	}
		
	InitializeCallbacks();
}

void __lamp_main( int argc, char** argv, char** envp )
{
	const int status = main( argc, argv, envp );
	
	exit( status );
}

#pragma force_active reset

#endif

