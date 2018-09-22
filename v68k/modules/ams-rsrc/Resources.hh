/*
	Resources.hh
	------------
*/

#ifndef RESOURCES_HH
#define RESOURCES_HH

pascal void RsrcZoneInit_patch();

pascal short OpenResFile_patch( const unsigned char* name );

pascal short ResError_patch();
pascal short CurResFile_patch();

pascal void UseResFile_patch( short refnum );

pascal char** GetResource_patch( unsigned long type, short id );

pascal char** GetNamedResource_patch( unsigned long         type,
                                      const unsigned char*  name );

pascal void LoadResource_patch( char** resource );

pascal void ReleaseResource_patch( char** resource );

pascal void DetachResource_patch( char** resource );

pascal short GetResAttrs_patch( char** resource );

pascal long SizeRsrc_patch( char** resource );

#endif
