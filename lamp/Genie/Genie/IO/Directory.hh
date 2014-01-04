/*	============
 *	Directory.hh
 *	============
 */

#ifndef GENIE_IO_DIRECTORY_HH
#define GENIE_IO_DIRECTORY_HH

// POSIX
#include "dirent.h"
#include "errno.h"

// vfs
#include "vfs/dir_contents_box.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"
#include "Genie/IO/Base.hh"
#include "Genie/IO/IOHandle_Cast.hh"


namespace Genie
{
	
	class DirHandle : public IOHandle
	{
		private:
			vfs::dir_contents_box  its_contents;
		
		public:
			DirHandle( const vfs::node* dir, vfs::filehandle_destructor dtor = NULL );
			
			~DirHandle();
			
			bool IsDirectory() const  { return true; }
			
			int ReadDir( dirent& entry );
	};
	
	template <> struct IOHandle_Downcast_Traits< DirHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsDirectory; }
		
		static int GetError( IOHandle& )  { return ENOTDIR; }
	};
	
}

#endif

