/*	=======
 *	Pipe.hh
 *	=======
 */

#ifndef GENIE_IO_PIPE_HH
#define GENIE_IO_PIPE_HH

// Boost
#include <boost/shared_ptr.hpp>

// Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	class PipeState
	{
		private:
			std::list< std::string > itsStrings;
			bool itsInputHasClosed;
			bool itsOutputHasClosed;
		
		public:
			PipeState() : itsInputHasClosed ( false ),
			              itsOutputHasClosed( false )
			{
			}
			
			bool IsReadable() const;
			bool IsWritable() const;
			
			bool InputHasClosed()  const  { return itsInputHasClosed;  }
			bool OutputHasClosed() const  { return itsOutputHasClosed; }
			
			bool CloseInput()   { itsInputHasClosed  = true;  return itsOutputHasClosed; }
			bool CloseOutput()  { itsOutputHasClosed = true;  return itsInputHasClosed;  }
			
			int Read (       char* data, std::size_t byteCount, bool blocking );
			int Write( const char* data, std::size_t byteCount, bool blocking );
	};
	
	class PipeInHandle : public StreamHandle
	{
		private:
			boost::shared_ptr< PipeState > state;
			bool itIsBlocking;
		
		public:
			static TypeCode Type()  { return kPipeType; }
			
			virtual TypeCode ActualType() const  { return Type(); }
			
			PipeInHandle( boost::shared_ptr< PipeState > state ) : state       ( state ),
			                                                       itIsBlocking( true  )
			{
			}
			
			~PipeInHandle();
			
			unsigned int SysPoll() const
			{
				return   kPollWrite  * state->IsWritable()
				       | kPollExcept * state->OutputHasClosed();
			}
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount )
			{
				return state->Write( data, byteCount, itIsBlocking );
			}
			
			//void IOCtl( unsigned long request, int* argp );
			
			bool IsBlocking() const  { return itIsBlocking; }
			
			void SetBlocking   ()  { itIsBlocking = true;  }
			void SetNonBlocking()  { itIsBlocking = false; }
	};
	
	class PipeOutHandle : public StreamHandle
	{
		private:
			boost::shared_ptr< PipeState > state;
			bool itIsBlocking;
		
		public:
			static TypeCode Type()  { return kPipeType; }
			
			virtual TypeCode ActualType() const  { return Type(); }
			
			PipeOutHandle( boost::shared_ptr< PipeState > state ) : state       ( state ),
			                                                        itIsBlocking( true  )
			{
			}
			
			~PipeOutHandle();
			
			unsigned int SysPoll() const
			{
				return   kPollRead   * state->IsReadable()
				       | kPollExcept * state->InputHasClosed();
			}
			
			int SysRead( char* data, std::size_t byteCount )
			{
				return state->Read( data, byteCount, itIsBlocking );
			}
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			bool IsBlocking() const  { return itIsBlocking; }
			
			void SetBlocking   ()  { itIsBlocking = true;  }
			void SetNonBlocking()  { itIsBlocking = false; }
	};
	
}

#endif

