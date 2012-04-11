/*
	file_method_set.hh
	------------------
*/

#ifndef GENIE_FS_FILEMETHODSET_HH
#define GENIE_FS_FILEMETHODSET_HH

// Genie
#include "Genie/code/shared_exec_handle.hh"
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	class FSTree;
	
	typedef void (*attach_method  )( const FSTree*, const FSTree* );
	typedef void (*copyfile_method)( const FSTree*, const FSTree* );
	typedef void (*hardlink_method)( const FSTree*, const FSTree* );
	
	typedef shared_exec_handle (*loadexec_method)( const FSTree* );
	
	
	struct file_method_set
	{
		attach_method    attach;
		copyfile_method  copyfile;
		hardlink_method  hardlink;
		loadexec_method  loadexec;
	};
	
}

#endif

