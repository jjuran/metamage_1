/*	=======
 *	Pipe.hh
 *	=======
 */

#ifndef GENIE_IO_PIPE_HH
#define GENIE_IO_PIPE_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/conduit.hh"

// Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	class PipeInHandle : public StreamHandle
	{
		private:
			boost::intrusive_ptr< plus::conduit > itsConduit;
		
		public:
			PipeInHandle( const boost::intrusive_ptr< plus::conduit >&  conduit,
			              bool                                          nonblocking );
			
			~PipeInHandle();
			
			unsigned int SysPoll();
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
	};
	
	class PipeOutHandle : public StreamHandle
	{
		private:
			boost::intrusive_ptr< plus::conduit > itsConduit;
		
		public:
			PipeOutHandle( const boost::intrusive_ptr< plus::conduit >&  conduit,
			               bool                                          nonblocking );
			
			~PipeOutHandle();
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
	};
	
}

#endif

