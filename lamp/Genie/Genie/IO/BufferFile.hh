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
#include "Pedestal/Scroller.hh"
#include "Pedestal/TEView.hh"
#include "Pedestal/Window.hh"

// Genie
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/Window.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	class BufferWindow : public Ped::Window< Ped::Scroller< Ped::TEView, Ped::kLiveFeedbackVariant > >,
	                     public WindowHandle
	{
		private:
			std::size_t  itsMark;
			bool         itHasReceivedEOF;
		
		public:
			typedef Ped::Window< Ped::Scroller< Ped::TEView, Ped::kLiveFeedbackVariant > > Base;
			
			BufferWindow( TerminalID id );
			
			~BufferWindow();
			
			Pedestal::WindowCore& GetWindowCore()  { return *this; }
			
			Nitrogen::WindowRef GetWindowRef() const  { return Get(); }
			
			bool KeyDown( const EventRecord& event );
			
			bool ReceivedEOF() const  { return itHasReceivedEOF; }
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			off_t Seek( off_t offset, int whence );
			
			off_t GetEOF() const;
			
			void SetEOF( off_t length );
	};
	
	
	class BufferFileHandle : public RegularFileHandle
	{
		private:
			boost::shared_ptr< IOHandle >  itsWindow;
			std::string                    itsData;
			
		
		public:
			BufferFileHandle( TerminalID id );
			
			~BufferFileHandle();
			
			IOHandle* Next() const;
			
			FSTreePtr GetFile() const  { return itsWindow->GetFile(); }
			
			unsigned int SysPoll() const
			{
				unsigned readability = static_cast< BufferWindow* >( itsWindow.get() )->ReceivedEOF() ? kPollRead : 0;
				
				return readability | kPollWrite;
			}
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount )  { return static_cast< BufferWindow* >( itsWindow.get() )->SysWrite( data, byteCount ); }
			
			off_t Seek( off_t offset, int whence )  { return static_cast< BufferWindow* >( itsWindow.get() )->Seek( offset, whence ); }
			
			off_t GetEOF() const  { return static_cast< BufferWindow* >( itsWindow.get() )->GetEOF(); }
			
			void SetEOF( off_t length )  { static_cast< BufferWindow* >( itsWindow.get() )->SetEOF( length ); }
	};
	
	boost::shared_ptr< IOHandle > NewBufferFile();
	
}

#endif

