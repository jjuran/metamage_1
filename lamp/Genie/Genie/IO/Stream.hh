/*	=========
 *	Stream.hh
 *	=========
 */

#ifndef GENIE_IO_STREAM_HH
#define GENIE_IO_STREAM_HH

// Standard C++
#include <string>

// POSIX
#include "errno.h"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	class StreamHandle : public IOHandle
	{
		private:
			std::string peekBuffer;
		
		public:
			virtual ~StreamHandle();
			
			bool IsStream() const  { return true; }
			
			virtual unsigned int SysPoll() const = 0;
			
			virtual int SysRead( char* data, std::size_t byteCount ) = 0;
			
			virtual int SysWrite( const char* data, std::size_t byteCount ) = 0;
			
			virtual void IOCtl( unsigned long request, int* argp );
			
			virtual bool IsDisconnected() const  { return false; }
			
			virtual bool IsNonblocking() const = 0;
			
			virtual void SetNonblocking  () = 0;
			virtual void ClearNonblocking() = 0;
			
			void TryAgainLater() const;
			
			const std::string& Peek( ByteCount minBytes );
			
			unsigned int Poll() const;
			
			int Read( char* data, std::size_t byteCount );
			
			int Write( const char* data, std::size_t byteCount );
	};
	
	template <> struct IOHandle_Downcast_Traits< StreamHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsStream; }
		
		static int GetError( IOHandle& handle )  { return handle.IsDirectory() ? EISDIR : EINVAL; }
	};
	
}

#endif

