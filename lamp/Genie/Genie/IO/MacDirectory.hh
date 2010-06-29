/*
	MacDirectory.hh
	---------------
*/

#ifndef GENIE_IO_VIRTUALDIRECTORY_HH
#define GENIE_IO_VIRTUALDIRECTORY_HH

// Nitrogen
#include "Mac/Files/Types/FSDirSpec.hh"

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

