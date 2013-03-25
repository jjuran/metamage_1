/*
	file_memory_mapping.hh
	----------------------
*/

#ifndef VFS_MMAP_TYPES_FILEMEMORYMAPPING_HH
#define VFS_MMAP_TYPES_FILEMEMORYMAPPING_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/memory_mapping.hh"
#include "vfs/memory_mapping_ptr.hh"


namespace vfs
{
	
	class file_memory_mapping : public memory_mapping
	{
		private:
			memory_mapping_ptr  its_memory;
			filehandle_ptr      its_file;
		
		public:
			file_memory_mapping( memory_mapping*  memory,
			                     filehandle*      file );
			
			~file_memory_mapping();
			
			filehandle* get_file() const  { return its_file.get(); }
	};
	
}

#endif

