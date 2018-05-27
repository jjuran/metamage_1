/*
	Segments.hh
	-----------
*/

#ifndef SEGMENTS_HH
#define SEGMENTS_HH

pascal void LoadSeg_patch( short segnum );

pascal short Launch_patch( struct LaunchParamBlockRec* pb : __A0 ) : __D0;

pascal void ExitToShell_patch();

#endif
