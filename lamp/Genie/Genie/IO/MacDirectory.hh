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
		
		public:
			MacDirHandle( const Mac::FSDirSpec& dir )
			:
				its_dir_spec( dir )
			{
			}
			
			FSTreePtr GetFile();
	};
	
}

#endif

