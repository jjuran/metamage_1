/*
	VirtualDirectory.hh
	-------------------
*/

#ifndef GENIE_IO_VIRTUALDIRECTORY_HH
#define GENIE_IO_VIRTUALDIRECTORY_HH

// Genie
#include "Genie/IO/Directory.hh"


namespace Genie
{
	
	class VirtualDirHandle : public DirHandle
	{
		private:
			FSTreePtr its_dir;
		
		public:
			VirtualDirHandle( const FSTreePtr& tree );
			
			~VirtualDirHandle();
			
			FSTreePtr GetFile();
	};
	
}

#endif

