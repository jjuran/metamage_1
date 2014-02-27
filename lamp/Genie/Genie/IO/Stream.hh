/*	=========
 *	Stream.hh
 *	=========
 */

#ifndef GENIE_IO_STREAM_HH
#define GENIE_IO_STREAM_HH

// POSIX
#include "errno.h"

// Genie
#include "Genie/IO/Base.hh"
#include "Genie/IO/IOHandle_Cast.hh"


namespace Genie
{
	
	class StreamHandle : public IOHandle
	{
		public:
			StreamHandle( int                                flags,
			              const vfs::filehandle_method_set*  methods = NULL );
			
			StreamHandle( const vfs::node*                   file,
			              int                                flags,
			              const vfs::filehandle_method_set*  methods = NULL );
			
			virtual ~StreamHandle();
			
			bool IsStream() const  { return true; }
			
			virtual unsigned int SysPoll()  { return kPollRead | kPollWrite; }
			
			virtual ssize_t SysRead( char* data, std::size_t byteCount );
			
			virtual ssize_t SysWrite( const char* data, std::size_t byteCount );
			
			void TryAgainLater() const;
			
			unsigned int Poll();
			
			ssize_t Read( char* data, std::size_t byteCount );
			
			ssize_t Write( const char* data, std::size_t byteCount );
	};
	
	template <> struct IOHandle_Downcast_Traits< StreamHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsStream; }
		
		static int GetError( IOHandle& handle )  { return handle.IsDirectory() ? EISDIR : EINVAL; }
	};
	
}

#endif

