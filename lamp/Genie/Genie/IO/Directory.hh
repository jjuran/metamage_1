/*	============
 *	Directory.hh
 *	============
 */

#ifndef GENIE_IO_DIRECTORY_HH
#define GENIE_IO_DIRECTORY_HH

// vfs
#include "vfs/dir_contents_box.hh"
#include "vfs/filehandle.hh"
#include "vfs/node_fwd.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"

// POSIX
//#include <dirent.h>
struct dirent;


namespace vfs
{
	
	struct filehandle_method_set;
	
	extern const stream_method_set dir_stream_methods;
	
	
	class dir_handle : public filehandle
	{
		private:
			dir_contents_box  its_contents;
		
		public:
			dir_handle( const node* dir, filehandle_destructor dtor = NULL );
			
			dir_handle( const filehandle_method_set& methods );
			
			~dir_handle();
			
			int readdir( dirent& entry );
	};
	
}

#endif
