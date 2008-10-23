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

// Pedestal
#include "Pedestal/UserWindow.hh"

// Genie
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/Window.hh"


namespace Genie
{
	
	class BufferWindow : public Pedestal::UserWindow,
	                     public WindowHandle
	{
		public:
			typedef Pedestal::UserWindow Base;
			
			BufferWindow( TerminalID id, const std::string& name );
			
			Pedestal::WindowCore& GetWindowCore()  { return *this; }
			
			Nitrogen::WindowRef GetWindowRef() const  { return Get(); }
			
	};
	
	
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
			
			off_t Seek( off_t offset, int whence );
			
			off_t GetEOF() const;
			
			void SetEOF( off_t length );
	};
	
	boost::shared_ptr< IOHandle > NewBufferFile();
	
}

#endif

