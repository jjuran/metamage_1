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

// Genie
#include "Genie/IO/Directory.hh"


namespace Genie
{
	
	class MacDirHandle : public DirHandle
	{
		private:
			Mac::FSDirSpec its_dir_spec;
			bool           it_is_on_server;
		
		public:
			MacDirHandle( const Mac::FSDirSpec& dir, bool on_server )
			:
				its_dir_spec( dir ),
				it_is_on_server( on_server )
			{
			}
			
			FSTreePtr GetFile();
	};
	
}

#endif

