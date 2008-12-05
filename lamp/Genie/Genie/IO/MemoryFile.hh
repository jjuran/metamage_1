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


namespace Genie
{
	
	class MemoryFileHandle : public RegularFileHandle
	{
		private:
			FSTreePtr    itsFile;
			::Ptr        itsBase;  // base address
			std::size_t  itsSize;
		
		public:
			MemoryFileHandle( const FSTreePtr&  file,
			                  ::Ptr             base,
			                  std::size_t       size ) : itsFile( file ),
			                                             itsBase( base ),
			                                             itsSize( size )
			{
			}
			
			~MemoryFileHandle();
			
			FSTreePtr GetFile() const  { return itsFile; }
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			off_t GetEOF() const  { return itsSize; }
	};
	
}

#endif

