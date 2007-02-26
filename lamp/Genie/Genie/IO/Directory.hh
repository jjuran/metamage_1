/*	============
 *	Directory.hh
 *	============
 */

#ifndef GENIE_IO_DIRECTORY_HH
#define GENIE_IO_DIRECTORY_HH

// POSIX
#include "dirent.h"

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
			static TypeCode Type()  { return kDirectoryType; }
			
			DirHandle( const FSTreePtr& tree );
			
			TypeCode ActualType() const  { return Type(); }
			
			FSTreePtr GetFile() const  { return itsDir; }
			
			const dirent* ReadDir();
			
			void RewindDir();
			void SeekDir( off_t index );
			off_t TellDir() const;
	};
	
}

#endif

