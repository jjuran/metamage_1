/*
	poll_result.hh
	--------------
*/

#ifndef VFS_ENUM_POLLRESULT_HH
#define VFS_ENUM_POLLRESULT_HH


namespace vfs
{
	
	enum
	{
		Poll_read   = 1,
		Poll_write  = 2,
		Poll_except = 4
	};
	
}

#endif

