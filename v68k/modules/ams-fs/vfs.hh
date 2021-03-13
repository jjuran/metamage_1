/*
	vfs.hh
	------
*/

#ifndef VFS_HH
#define VFS_HH

// Standard
#include <stdint.h>


struct FCB;
struct HFileParam;
struct VCB;

typedef short OSErr;


template < class Entry >
struct filesystem_vtable
{
	typedef const Entry* (*lookup_proc)( VCB* vcb, const uint8_t* name );
	typedef const Entry* (*getnth_proc)( VCB* vcb, short n );
	
	typedef OSErr (*Close_proc)( FCB* fcb );
	typedef OSErr (*Create_proc)( VCB* vcb, const uint8_t* name );
	typedef OSErr (*openfork_proc)( short trap_word, FCB* fcb, const Entry* e );
	typedef OSErr (*FlushFile_proc)( FCB* fcb );
	typedef OSErr (*GetFileInfo_proc)( HFileParam* pb, const Entry* entry );
	
	typedef GetFileInfo_proc SetFileInfo_proc;
	
	lookup_proc  lookup;
	getnth_proc  get_nth;
	
	Close_proc        Close;
	Create_proc       Create;
	openfork_proc     open_fork;
	FlushFile_proc    FlushFile;
	GetFileInfo_proc  GetFileInfo;
	SetFileInfo_proc  SetFileInfo;
};

typedef filesystem_vtable< struct generic_file_entry > vfs_table;

const vfs_table* vfs_from_vcb( const VCB* vcb );

#endif
