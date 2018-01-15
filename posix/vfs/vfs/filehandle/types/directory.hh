/*
	directory.hh
	------------
*/

#ifndef VFS_FILEHANDLE_TYPES_DIRECTORY_HH
#define VFS_FILEHANDLE_TYPES_DIRECTORY_HH

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node_fwd.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"


namespace vfs
{
	
	class dir_contents;
	
	extern const stream_method_set dir_stream_methods;
	
	
	struct dir_handle_extra
	{
		filehandle_destructor  chained_destructor;
		dir_contents*          contents;
	};
	
	void destroy_dir_handle( filehandle* that );
	
	
	class dir_handle : public filehandle
	{
		public:
			dir_handle( const node* dir, filehandle_destructor dtor = 0 );  // NULL
	};
	
}

#endif
