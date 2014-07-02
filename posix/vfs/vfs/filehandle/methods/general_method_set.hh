/*
	general_method_set.hh
	---------------------
*/

#ifndef VFS_FILEHANDLE_METHODS_GENERALMETHODSET_HH
#define VFS_FILEHANDLE_METHODS_GENERALMETHODSET_HH

// vfs
#include "vfs/memory_mapping_ptr.hh"
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	class filehandle;
	
	
	typedef memory_mapping_ptr (*mmap_method)( filehandle* that, size_t length, int prot, int flags, off_t offset );
	
	typedef void (*ioctl_method)( filehandle*, unsigned long request, int* argp );
	
	typedef node_ptr (*getfile_method)( filehandle* that );
	
	typedef void (*conjoin_method)( filehandle& that, filehandle& target );
	
	
	struct general_method_set
	{
		mmap_method     mmap;
		ioctl_method    ioctl;
		getfile_method  getfile;
		conjoin_method  conjoin;
	};
	
}

#endif
