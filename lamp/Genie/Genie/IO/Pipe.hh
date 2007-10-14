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
			boost::shared_ptr< Conduit > itsConduit;
			bool itIsBlocking;
		
		public:
			static TypeCode Type()  { return kPipeType; }
			
			virtual TypeCode ActualType() const  { return Type(); }
			
			PipeInHandle( boost::shared_ptr< Conduit > conduit ) : itsConduit  ( conduit ),
			                                                       itIsBlocking( true    )
			{
			}
			
			~PipeInHandle();
			
			unsigned int SysPoll() const
			{
				return   kPollWrite  * itsConduit->IsWritable()
				       | kPollExcept * itsConduit->OutputHasClosed();
			}
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount )
			{
				return itsConduit->Write( data, byteCount, itIsBlocking );
			}
			
			//void IOCtl( unsigned long request, int* argp );
			
			bool IsBlocking() const  { return itIsBlocking; }
			
			void SetBlocking   ()  { itIsBlocking = true;  }
			void SetNonBlocking()  { itIsBlocking = false; }
	};
	
	class PipeOutHandle : public StreamHandle
	{
		private:
			boost::shared_ptr< Conduit > itsConduit;
			bool itIsBlocking;
		
		public:
			static TypeCode Type()  { return kPipeType; }
			
			virtual TypeCode ActualType() const  { return Type(); }
			
			PipeOutHandle( boost::shared_ptr< Conduit > conduit ) : itsConduit  ( conduit ),
			                                                        itIsBlocking( true    )
			{
			}
			
			~PipeOutHandle();
			
			unsigned int SysPoll() const
			{
				return   kPollRead   * itsConduit->IsReadable()
				       | kPollExcept * itsConduit->InputHasClosed();
			}
			
			int SysRead( char* data, std::size_t byteCount )
			{
				return itsConduit->Read( data, byteCount, itIsBlocking );
			}
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			bool IsBlocking() const  { return itIsBlocking; }
			
			void SetBlocking   ()  { itIsBlocking = true;  }
			void SetNonBlocking()  { itIsBlocking = false; }
	};
	
}

#endif

