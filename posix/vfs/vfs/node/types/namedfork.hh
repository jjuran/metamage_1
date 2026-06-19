/*
	namedfork.hh
	------------
*/

#ifndef VFS_NODE_TYPES_NAMEDFORK_HH
#define VFS_NODE_TYPES_NAMEDFORK_HH


namespace vfs
{
	
	enum special_info
	{
		Info_null,
		Info_PkgInfo,
		Info_FInfo,
		Info_GetFInfo,
		Info_SetFInfo,
	};
	
	extern const unsigned info_sizes[];
	
}

#endif
