/*
	documents.hh
	------------
*/

#ifndef DOCUMENTS_HH
#define DOCUMENTS_HH


struct FCB;
struct HFileParam;
struct VCB;

typedef unsigned char  Byte;
typedef short          OSErr;
typedef unsigned long  UInt32;


extern int docfs_fd;

void mount_virtual_documents_volume();

const Byte* documents_get_nth( VCB* vcb, short n );

OSErr documents_Close( FCB* fcb );
OSErr documents_Write( FCB* fcb, const char* buffer, UInt32 length );

OSErr documents_Create( VCB* vcb, const Byte* name );

OSErr documents_open_fork( short trap_word, FCB* fcb, const Byte* name );

OSErr documents_FlushFile( FCB* fcb );

OSErr documents_GetFileInfo( HFileParam* pb, const Byte* name );
OSErr documents_SetFileInfo( HFileParam* pb, const Byte* name );

#endif
