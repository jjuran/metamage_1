/*
	Segments.hh
	-----------
*/

#ifndef SEGMENTS_HH
#define SEGMENTS_HH

pascal void GetAppParms_patch( unsigned char* name, short* refNum, char*** hp );

pascal void LoadSeg_patch( short segnum );

pascal void UnloadSeg_patch( void* ptr );

short Launch_patch( struct LaunchParamBlockRec* pb : __A0 );

void ExitToShell_patch();

#endif
