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
			::Ptr        itsBase;  // base address
			std::size_t  itsSize;
		
		public:
			MemoryFileHandle( const FSTreePtr&  file,
			                  OpenFlags         flags,
			                  ::Ptr             base,
			                  std::size_t       size ) : VirtualFileHandle( file, flags ),
			                                             itsBase( base ),
			                                             itsSize( size )
			{
			}
			
			~MemoryFileHandle();
			
			boost::shared_ptr< IOHandle > Clone();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			off_t GetEOF() const  { return itsSize; }
	};
	
}

#endif

