/*	============
 *	QueryFile.hh
 *	============
 */

#ifndef GENIE_IO_QUERYFILE_HH
#define GENIE_IO_QUERYFILE_HH

// Standard C++
#include <string>

// POSIX
#include "sys/types.h"

// Genie
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	class QueryFileHandle : public VirtualFileHandle
	{
		private:
			std::string itsData;
		
		public:
			QueryFileHandle( const FSTreePtr&    file,
			                 const std::string&  data ) : VirtualFileHandle( file ),
			                                              itsData( data )
			{
			}
			
			~QueryFileHandle();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			off_t GetEOF() const  { return itsData.size(); }
	};
	
}

#endif

