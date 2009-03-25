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

#pragma force_active reset

#endif

