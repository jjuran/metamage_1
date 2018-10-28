/*
	Resources.hh
	------------
*/

#ifndef RESOURCES_HH
#define RESOURCES_HH

pascal void RsrcZoneInit_patch();

pascal short ResError_patch();
pascal short CurResFile_patch();

pascal void UseResFile_patch( short refnum );

pascal char** GetResource_patch( unsigned long type, short id );

pascal void ReleaseResource_patch( char** resource );

pascal void DetachResource_patch( char** resource );

pascal long SizeRsrc_patch( char** resource );

#endif
