/*
	MacDirectory.hh
	---------------
*/

#ifndef GENIE_IO_VIRTUALDIRECTORY_HH
#define GENIE_IO_VIRTUALDIRECTORY_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSDIRSPEC_HH
#include "Mac/Files/Types/FSDirSpec.hh"
#endif

// vfs
#include "vfs/node_ptr.hh"
#include "vfs/filehandle/types/directory.hh"


namespace Genie
{
	
	class MacDirHandle : public vfs::dir_handle
	{
		private:
			Mac::FSDirSpec its_dir_spec;
		
		public:
			MacDirHandle( const Mac::FSDirSpec& dir );
			
			vfs::node_ptr GetFile();
	};
	
}

#endif

