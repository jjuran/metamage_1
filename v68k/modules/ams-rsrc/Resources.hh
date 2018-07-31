/*
	Resources.hh
	------------
*/

#ifndef RESOURCES_HH
#define RESOURCES_HH

pascal char** GetResource_patch( unsigned long type, short id );

pascal void ReleaseResource_patch( char** res );

pascal void DetachResource_patch( char** res );

#endif
