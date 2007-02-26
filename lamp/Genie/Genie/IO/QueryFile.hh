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
#include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	class QueryFileHandle : public RegularFileHandle
	{
		private:
			FSTreePtr    itsFile;
			std::string  itsData;
			std::size_t  itsMark;
		
		public:
			static TypeCode Type()  { return kQueryFileType; }
			
			QueryFileHandle( const FSTreePtr&    file,
			                 const std::string&  data ) : itsFile( file ),
			                                              itsData( data ),
			                                              itsMark(      )  {}
			
			TypeCode ActualType() const  { return Type(); }
			
			FSTreePtr GetFile() const  { return itsFile; }
			
			unsigned int SysPoll() const  { return kPollRead | kPollWrite | kPollExcept; }
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			off_t Seek( off_t offset, int whence );
	};
	
}

#endif

