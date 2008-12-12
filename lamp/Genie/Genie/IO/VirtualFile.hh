/*	==============
 *	VirtualFile.hh
 *	==============
 */

#ifndef GENIE_IO_VIRTUALFILE_HH
#define GENIE_IO_VIRTUALFILE_HH

// Genie
#include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	class VirtualFileHandle : public RegularFileHandle
	{
		private:
			FSTreePtr itsFile;
		
		public:
			VirtualFileHandle( const FSTreePtr&  file,
			                   OpenFlags         flags ) : RegularFileHandle( flags ),
			                                               itsFile( file )
			{
			}
			
			FSTreePtr GetFile() const  { return itsFile; }
	};
	
}

#endif

