/*	============
 *	Directory.hh
 *	============
 */

#ifndef GENIE_IO_DIRECTORY_HH
#define GENIE_IO_DIRECTORY_HH

// POSIX
#include "dirent.h"
#include "errno.h"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	class DirHandle : public IOHandle
	{
		private:
			FSTreePtr      itsDir;
			FSIteratorPtr  itsIterator;
		
		private:
			const FSIteratorPtr& Iterator();
		
		public:
			DirHandle( const FSTreePtr& tree );
			
			~DirHandle();
			
			bool IsDirectory() const  { return true; }
			
			FSTreePtr GetFile()  { return itsDir; }
			
			off_t Seek( off_t offset, int whence );
			
			int ReadDir( dirent& entry );
	};
	
	template <> struct IOHandle_Downcast_Traits< DirHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsDirectory; }
		
		static int GetError( IOHandle& )  { return ENOTDIR; }
	};
	
}

#endif

