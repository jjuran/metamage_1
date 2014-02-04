/*	============
 *	Directory.hh
 *	============
 */

#ifndef GENIE_IO_DIRECTORY_HH
#define GENIE_IO_DIRECTORY_HH

// POSIX
#include "dirent.h"

// vfs
#include "vfs/dir_contents_box.hh"
#include "vfs/filehandle.hh"
#include "vfs/node_fwd.hh"


namespace Genie
{
	
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

