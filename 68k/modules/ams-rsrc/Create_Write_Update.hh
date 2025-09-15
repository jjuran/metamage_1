/*
	Create_Write_Update.hh
	----------------------
*/

#ifndef CREATEWRITEUPDATE_HH
#define CREATEWRITEUPDATE_HH


typedef unsigned char Byte;

typedef char** Handle;


pascal void CreateResFile_patch( const Byte* name );

pascal void UpdateResFile_patch( short refnum );

pascal void WriteResource_patch( Handle resource );

#endif
