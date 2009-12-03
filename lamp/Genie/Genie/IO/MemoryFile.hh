/*	=============
 *	MemoryFile.hh
 *	=============
 */

#ifndef GENIE_IO_MEMORYFILE_HH
#define GENIE_IO_MEMORYFILE_HH

// Standard C++
#include <string>

// POSIX
#include "sys/types.h"

// Genie
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	class MemoryFileHandle : public VirtualFileHandle< RegularFileHandle >
	{
		private:
			char*        itsBase;  // base address
			std::size_t  itsSize;
		
		public:
			MemoryFileHandle( const FSTreePtr&  file,
			                  OpenFlags         flags,
			                  char*             base,
			                  std::size_t       size )
			:
				VirtualFileHandle< RegularFileHandle >( file, flags ),
				itsBase( base ),
				itsSize( size )
			{
			}
			
			~MemoryFileHandle();
			
			boost::shared_ptr< IOHandle > Clone();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			//void IOCtl( unsigned long request, int* argp );
			
			off_t GetEOF()  { return itsSize; }
			
			boost::shared_ptr< memory_mapping > Map( size_t length, off_t offset );
	};
	
}

#endif

