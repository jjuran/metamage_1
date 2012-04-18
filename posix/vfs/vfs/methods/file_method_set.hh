/*
	file_method_set.hh
	------------------
*/

#ifndef VFS_METHODS_FILEMETHODSET_HH
#define VFS_METHODS_FILEMETHODSET_HH

// vfs
#include "vfs/node_fwd.hh"
#include "vfs/program_ptr.hh"


namespace vfs
{
	
	typedef void (*attach_method  )( const node*, const node* );
	typedef void (*copyfile_method)( const node*, const node* );
	typedef void (*hardlink_method)( const node*, const node* );
	
	typedef program_ptr (*loadexec_method)( const node* );
	
	
	struct file_method_set
	{
		attach_method    attach;
		copyfile_method  copyfile;
		hardlink_method  hardlink;
		loadexec_method  loadexec;
	};
	
}

#endif

