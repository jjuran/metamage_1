/*
	MFS.hh
	------
*/

#ifndef MOUNT_HH
#define MOUNT_HH


struct FCB;
struct HFileParam;
struct VCB;

typedef unsigned char   Byte;
typedef short           OSErr;
typedef signed char     SInt8;
typedef unsigned char   UInt8;
typedef unsigned short  UInt16;
typedef unsigned long   UInt32;


namespace mfs
{
	
#pragma options align=packed
	
	struct f_info
	{
		UInt16 words[ 8 ];
	};
	
	struct file_directory_entry
	{
		SInt8   flAttrib;
		UInt8   flVersNum;
		f_info  flFndrInfo;
		UInt32  flNum;
		UInt16  flStBlk;
		UInt32  flLgLen;
		UInt32  flPyLen;
		UInt16  flRStBlk;
		UInt32  flRLgLen;
		UInt32  flRPyLen;
		UInt32  flCrDat;
		UInt32  flMdDat;
		Byte    flNam[ 256 ];
	};
	
	typedef file_directory_entry _fde;
	
#pragma options align=reset
	
}

const mfs::file_directory_entry* MFS_get_nth( VCB* vcb, short n );

const mfs::file_directory_entry* MFS_lookup( VCB* vcb, const Byte* name );

void MFS_load( VCB* vcb, UInt16 stBlk, char* buffer, short n );

OSErr MFS_open_fork( short trap_word, FCB* fcb, const mfs::_fde* entry );

OSErr MFS_GetFileInfo( HFileParam* pb, const mfs::_fde* entry );

#endif
