/*	=======
 *	Pipe.hh
 *	=======
 */

#ifndef GENIE_IO_PIPE_HH
#define GENIE_IO_PIPE_HH

// Boost
#include <boost/shared_ptr.hpp>

// Genie
#include "Genie/IO/Conduit.hh"
#include "Genie/IO/InternallyNonblocking.hh"
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	class PipeInHandle : public InternallyNonblocking< StreamHandle >
	{
		private:
			boost::shared_ptr< Conduit > itsConduit;
		
		public:
			PipeInHandle( boost::shared_ptr< Conduit > conduit ) : itsConduit( conduit )
			{
			}
			
			~PipeInHandle();
			
			unsigned int SysPoll() const
			{
				return   kPollRead
				       | kPollWrite * itsConduit->IsWritable();
			}
			
			int SysWrite( const char* data, std::size_t byteCount )
			{
				return itsConduit->Write( data, byteCount, IsNonblocking() );
			}
			
			//void IOCtl( unsigned long request, int* argp );
	};
	
	class PipeOutHandle : public InternallyNonblocking< StreamHandle >
	{
		private:
			boost::shared_ptr< Conduit > itsConduit;
		
		public:
			PipeOutHandle( boost::shared_ptr< Conduit > conduit ) : itsConduit( conduit )
			{
			}
			
			~PipeOutHandle();
			
			unsigned int SysPoll() const
			{
				return   kPollRead * itsConduit->IsReadable()
				       | kPollWrite;
			}
			
			int SysRead( char* data, std::size_t byteCount )
			{
				return itsConduit->Read( data, byteCount, IsNonblocking() );
			}
			
			//void IOCtl( unsigned long request, int* argp );
	};
	
}

#endif

