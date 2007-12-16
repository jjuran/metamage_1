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
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	class PipeInHandle : public StreamHandle
	{
		private:
			boost::shared_ptr< Conduit >  itsConduit;
			bool                          itIsNonblocking;
		
		public:
			PipeInHandle( boost::shared_ptr< Conduit > conduit ) : itsConduit     ( conduit ),
			                                                       itIsNonblocking( false   )
			{
			}
			
			~PipeInHandle();
			
			unsigned int SysPoll() const
			{
				return   kPollRead
				       | kPollWrite * itsConduit->IsWritable();
			}
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount )
			{
				return itsConduit->Write( data, byteCount, itIsNonblocking );
			}
			
			//void IOCtl( unsigned long request, int* argp );
			
			bool IsNonblocking() const  { return itIsNonblocking; }
			
			void SetNonblocking  ()  { itIsNonblocking = true;  }
			void ClearNonblocking()  { itIsNonblocking = false; }
	};
	
	class PipeOutHandle : public StreamHandle
	{
		private:
			boost::shared_ptr< Conduit >  itsConduit;
			bool                          itIsNonblocking;
		
		public:
			PipeOutHandle( boost::shared_ptr< Conduit > conduit ) : itsConduit     ( conduit ),
			                                                        itIsNonblocking( false   )
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
				return itsConduit->Read( data, byteCount, itIsNonblocking );
			}
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			bool IsNonblocking() const  { return itIsNonblocking; }
			
			void SetNonblocking  ()  { itIsNonblocking = true;  }
			void ClearNonblocking()  { itIsNonblocking = false; }
	};
	
}

#endif

