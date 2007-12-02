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
			
			BufferWindow( ConstStr255Param title );
			
			~BufferWindow();
			
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
			boost::shared_ptr< BufferWindow >  itsWindow;
			std::string  itsData;
			
		
		public:
			BufferFileHandle( const boost::shared_ptr< BufferWindow >&  window ) : itsWindow( window )
			{
			}
			
			~BufferFileHandle();
			
			IOHandle* Next() const;
			
			unsigned int SysPoll() const
			{
				unsigned readability = itsWindow->ReceivedEOF() ? kPollRead : 0;
				
				return readability | kPollWrite;
			}
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount )  { return itsWindow->SysWrite( data, byteCount ); }
			
			off_t Seek( off_t offset, int whence )  { return itsWindow->Seek( offset, whence ); }
			
			off_t GetEOF() const  { return itsWindow->GetEOF(); }
			
			void SetEOF( off_t length )  { itsWindow->SetEOF( length ); }
	};
	
	boost::shared_ptr< IOHandle > NewBufferFile();
	
}

#endif

