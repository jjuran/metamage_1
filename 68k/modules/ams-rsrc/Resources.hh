/*
	Resources.hh
	------------
*/

#ifndef RESOURCES_HH
#define RESOURCES_HH

typedef unsigned char Boolean;
typedef unsigned char Byte;

typedef unsigned long ResType;

typedef char** Handle;

typedef const Byte* ConstStr255Param;

typedef Byte Str255[ 256 ];


pascal void CreateResFile_patch( ConstStr255Param name );

pascal short OpenResFile_patch( ConstStr255Param name );

pascal void CloseResFile_patch( short refnum );

pascal short ResError_patch();
pascal short CurResFile_patch();

pascal short HomeResFile_patch( Handle resource );

pascal void UseResFile_patch( short refnum );

pascal void SetResLoad_patch( Boolean load );

pascal short CountResources_patch( ResType type );

pascal Handle GetIndResource_patch( ResType type, short index );

pascal Handle GetResource_patch( ResType type, short id );

pascal Handle GetNamedResource_patch( ResType           type,
                                      ConstStr255Param  name );

pascal void LoadResource_patch( Handle resource );

pascal void ReleaseResource_patch( Handle resource );

pascal void DetachResource_patch( Handle resource );

pascal void GetResInfo_patch( Handle    resource,
                              short*    id,
                              ResType*  type,
                              Str255    name );

pascal short GetResAttrs_patch( Handle resource );

pascal long SizeRsrc_patch( Handle resource );

pascal void SetResAttrs_patch( Handle resource, short attrs );

pascal void UpdateResFile_patch( short refnum );

pascal void WriteResource_patch( Handle resource );

pascal void SetResPurge_patch( Boolean install );

pascal short GetResFileAttrs_patch( short refnum );

pascal void SetResFileAttrs_patch( short refnum, short attrs );

pascal short Count1Types_patch();

pascal void Get1IxType_patch( ResType* type, short index );

pascal short Count1Resources_patch( ResType type );

pascal Handle Get1IxResource_patch( ResType type, short index );

pascal Handle Get1Resource_patch( ResType type, short id );

pascal Handle Get1NamedResource_patch( ResType           type,
                                       ConstStr255Param  name );

pascal short Unique1ID_patch( ResType type );

pascal short OpenRFPerm_patch( ConstStr255Param name, short vRefNum, char perm );

#endif
