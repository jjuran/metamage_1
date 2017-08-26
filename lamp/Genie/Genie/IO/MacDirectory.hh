/*
	MacDirectory.hh
	---------------
*/

#ifndef GENIE_IO_VIRTUALDIRECTORY_HH
#define GENIE_IO_VIRTUALDIRECTORY_HH

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// vfs
#include "vfs/node_ptr.hh"
#include "vfs/filehandle/types/directory.hh"


namespace Genie
{
	
	class MacDirHandle : public vfs::dir_handle
	{
		private:
			mac::types::VRefNum_DirID its_dir_spec;
		
		public:
			MacDirHandle( const mac::types::VRefNum_DirID& dir );
			
			vfs::node_ptr GetFile();
	};
	
}

#endif
