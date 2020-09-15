/*
	node_method_set.hh
	------------------
*/

#ifndef VFS_METHODS_NODEMETHODSET_HH
#define VFS_METHODS_NODEMETHODSET_HH


namespace vfs
{
	
	struct item_method_set;
	struct data_method_set;
	struct link_method_set;
	struct dir_method_set;
	struct file_method_set;
	struct misc_method_set;
	
	
	struct node_method_set
	{
		const item_method_set*  item_methods;
		const data_method_set*  data_methods;
		const link_method_set*  link_methods;
		const dir_method_set*   dir_methods;
		const file_method_set*  file_methods;
		const misc_method_set*  misc_methods;
	};
	
}

#endif
