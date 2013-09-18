/*
	terminal_method_set.hh
	----------------------
*/

#ifndef VFS_FILEHANDLE_METHODS_TERMINALMETHODSET_HH
#define VFS_FILEHANDLE_METHODS_TERMINALMETHODSET_HH


namespace vfs
{
	
	class filehandle;
	
	
	typedef void (*hangup_method)( filehandle* );
	
	struct terminal_method_set
	{
		hangup_method  hangup;
	};
	
}

#endif

