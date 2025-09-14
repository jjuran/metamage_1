/*
	Changed.hh
	----------
*/

#ifndef CHANGED_HH
#define CHANGED_HH


typedef unsigned char Byte;

typedef unsigned long ResType;

typedef char** Handle;


pascal void SetResInfo_patch( Handle resource, short id, const Byte* name );

pascal void ChangedResource_patch( Handle resource );

pascal void AddResource_patch( Handle       data,
                               ResType      type,
                               short        id,
                               const Byte*  name );

#endif
