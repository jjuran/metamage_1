/*	=============
 *	BufferFile.hh
 *	=============
 */

#ifndef GENIE_IO_BUFFERFILE_HH
#define GENIE_IO_BUFFERFILE_HH

// Standard C++
#include <string>

// POSIX
#include "sys/types.h"

// boost
#include <boost/shared_ptr.hpp>

// Genie
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/Window.hh"


namespace Genie
{
	
	class BufferFileHandle : public RegularFileHandle
	{
		private:
			boost::shared_ptr< IOHandle >  itsWindow;
			std::string                    itsData;
			
		
		public:
			static const char* PathPrefix()  { return "/sys/set/txt/"; }
			
			BufferFileHandle( TerminalID id, const std::string& name );
			
			~BufferFileHandle();
			
			IOHandle* Next() const;
			
			FSTreePtr GetFile() const  { return itsWindow->GetFile(); }
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			off_t GetEOF() const;
			
			void SetEOF( off_t length );
	};
	
}

#endif

