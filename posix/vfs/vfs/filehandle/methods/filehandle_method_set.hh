/*
	filehandle_method_set.hh
	------------------------
*/

#ifndef VFS_METHODS_FILEHANDLEMETHODSET_HH
#define VFS_METHODS_FILEHANDLEMETHODSET_HH


namespace vfs
{
	
	struct bstore_method_set;
	struct socket_method_set;
	
	
	struct filehandle_method_set
	{
		const bstore_method_set*  bstore_methods;
		const socket_method_set*  socket_methods;
	};
	
}

#endif

