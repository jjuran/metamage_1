/*
	Resources.hh
	------------
*/

#ifndef RESOURCES_HH
#define RESOURCES_HH

extern unsigned long module_A4;

pascal char** GetResource_patch( unsigned long type, short id );

pascal void ReleaseResource_patch( char** res );

#endif
