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
#include "Genie/FileSystem/FSTree.hh"
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	class DirHandle : public IOHandle
	{
		private:
			dirent fLastEntry;
			FSTreePtr      itsDir;
			FSIteratorPtr iterator;
		
		public:
			DirHandle( const FSTreePtr& tree );
			
			~DirHandle();
			
			bool IsDirectory() const  { return true; }
			
			FSTreePtr GetFile() const  { return itsDir; }
			
			off_t Seek( off_t offset, int whence );
			
			const dirent* ReadDir();
			
			void RewindDir();
			void SeekDir( off_t index );
			off_t TellDir() const;
	};
	
	template <> struct IOHandle_Downcast_Traits< DirHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsDirectory; }
		
		static int GetError( IOHandle& )  { return ENOTDIR; }
	};
	
}

#endif

