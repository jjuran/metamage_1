/*	=============
 *	InitTool68K.c
 *	=============
 */

#ifdef __MC68K__

extern void InitializeTool();

// Initialize environ from ToolScratch
extern void InitializeEnviron();

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();


#pragma force_active on

void InitializeTool()
{
	InitializeEnviron();
	
	InitializeCallbacks();
}

#pragma force_active reset

#endif

