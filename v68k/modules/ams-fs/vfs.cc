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
#include "MFS.hh"


static const filesystem_vtable< uint8_t > bootstrap_vtable =
{
	&bootstrap_lookup,
	&bootstrap_get_nth,
	
	NULL,
	&bootstrap_open_fork,
	NULL,
	&bootstrap_GetFileInfo,
};

static const filesystem_vtable< mfs::file_directory_entry > MFS_vtable =
{
	&MFS_lookup,
	&MFS_get_nth,
	
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
		
		case 0xD2D7:
			return (const vfs_table*) &MFS_vtable;
		
		default:
			return NULL;
	}
}
