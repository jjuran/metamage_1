/*
	vfs.cc
	------
*/

#include "vfs.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// ams-fs
#include "bootstrap.hh"
#include "documents.hh"
#include "macbinaryfs.hh"
#include "MFS.hh"
#include "remotefs.hh"


static const filesystem_vtable< Byte > bootstrap_vtable =
{
	&remotefs_lookup,
	&bootstrap_get_nth,
	
	NULL,
	NULL,
	NULL,
	&bootstrap_open_fork,
	NULL,
	&bootstrap_GetFileInfo,
};

static const filesystem_vtable< Byte > documents_vtable =
{
	&remotefs_lookup,
	&documents_get_nth,
	
	&documents_Close,
	&documents_Write,
	&documents_Create,
	&documents_open_fork,
	&documents_FlushFile,
	&documents_GetFileInfo,
	&documents_SetFileInfo,
};

static const filesystem_vtable< macbinary::header > MacBinary_vtable =
{
	&MacBinary_lookup,
	&MacBinary_get_nth,
	
	&MacBinary_Close,
	NULL,
	NULL,
	&MacBinary_open_fork,
	NULL,
	&MacBinary_GetFileInfo,
};

static const filesystem_vtable< mfs::file_directory_entry > MFS_vtable =
{
	&MFS_lookup,
	&MFS_get_nth,
	
	NULL,
	NULL,
	NULL,
	&MFS_open_fork,
	NULL,
	&MFS_GetFileInfo,
};

const vfs_table* vfs_from_vcb( const VCB* vcb )
{
	switch ( vcb->vcbSigWord )
	{
		case 'Ix':
			return (const vfs_table*) &bootstrap_vtable;
		
		case 0xD0C5:
		case 0x3e1a:
			return (const vfs_table*) &documents_vtable;
		
		case 'mB':
			return (const vfs_table*) &MacBinary_vtable;
		
		case 0xD2D7:
			return (const vfs_table*) &MFS_vtable;
		
		default:
			return NULL;
	}
}
