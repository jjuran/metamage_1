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


namespace Genie
{
	
	extern const vfs::stream_method_set dir_stream_methods;
	
	
	class DirHandle : public vfs::filehandle
	{
		private:
			vfs::dir_contents_box  its_contents;
		
		public:
			DirHandle( const vfs::node* dir, vfs::filehandle_destructor dtor = NULL );
			
			~DirHandle();
			
			bool IsDirectory() const  { return true; }
			
			int ReadDir( dirent& entry );
	};
	
}

#endif

