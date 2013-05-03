/*
	Handles.hh
	----------
*/

#ifndef HANDLES_HH
#define HANDLES_HH

pascal char** NewHandle_patch( long size : __D0, short trap_word : __D1 ) : __A0;

/*pascal*/ char** NewEmptyHandle_patch() : __A0;

pascal void DisposeHandle_patch( char** h : __A0 );

pascal void SetHandleSize_patch( char** h : __A0, long size : __D0, short trap_word : __D1 );

pascal long GetHandleSize_patch( char** h : __A0 ) : __D0;

pascal void ReallocateHandle_patch( char** h : __A0, long size : __D0, short trap_word : __D1 );

pascal void EmptyHandle_patch( char** h : __A0 );

#endif
