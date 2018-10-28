/*
	Resources.hh
	------------
*/

#ifndef RESOURCES_HH
#define RESOURCES_HH

pascal void RsrcZoneInit_patch();

pascal char** GetResource_patch( unsigned long type, short id );

pascal void ReleaseResource_patch( char** res );

pascal void DetachResource_patch( char** res );

pascal short ResError_patch();

pascal long SizeRsrc_patch( char** res );

#endif
