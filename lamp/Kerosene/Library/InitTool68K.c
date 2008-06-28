/*	============
 *	68KStartup.c
 *	============
 */

#ifdef __MC68K__

extern void InitializeTool();

// Initialize environ from ToolScratch
extern void InitializeEnviron();

#pragma force_active on

void InitializeTool()
{
	InitializeEnviron();
}

#pragma force_active reset

#endif

