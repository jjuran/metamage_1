/*
	Resources.hh
	------------
*/

#ifndef RESOURCES_HH
#define RESOURCES_HH

pascal short InitResources_patch();

pascal void RsrcZoneInit_patch();

pascal void CreateResFile_patch( const unsigned char* name );

pascal short OpenResFile_patch( const unsigned char* name );

pascal void CloseResFile_patch( short refnum );

pascal short ResError_patch();
pascal short CurResFile_patch();

pascal short HomeResFile_patch( char** resource );

pascal void UseResFile_patch( short refnum );

pascal void SetResLoad_patch( unsigned char load );

pascal short CountResources_patch( unsigned long type );

pascal char** GetIndResource_patch( unsigned long type, short index );

pascal char** GetResource_patch( unsigned long type, short id );

pascal char** GetNamedResource_patch( unsigned long         type,
                                      const unsigned char*  name );

pascal void LoadResource_patch( char** resource );

pascal void ReleaseResource_patch( char** resource );

pascal void DetachResource_patch( char** resource );

pascal void GetResInfo_patch( char**          resource,
                              short*          id,
                              unsigned long*  type,
                              unsigned char*  name );

pascal short GetResAttrs_patch( char** resource );

pascal long SizeRsrc_patch( char** resource );

pascal void SetResAttrs_patch( char** resource, short attrs );

pascal void ChangedResource_patch( char** resource );

pascal void AddResource_patch( char**                data,
                               unsigned long         type,
                               short                 id,
                               const unsigned char*  name );

pascal void UpdateResFile_patch( short refnum );

pascal void WriteResource_patch( char** resource );

pascal void SetResPurge_patch( unsigned char install );

pascal short Count1Resources_patch( unsigned long type );

pascal char** Get1IxResource_patch( unsigned long type, short index );

pascal char** Get1Resource_patch( unsigned long type, short id );

pascal char** Get1NamedResource_patch( unsigned long         type,
                                       const unsigned char*  name );

pascal short OpenRFPerm_patch( const unsigned char* name, short vRefNum, char perm );

#endif
