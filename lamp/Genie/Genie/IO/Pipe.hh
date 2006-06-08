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
			std::list< std::string > myStrings;
			std::string partialRead;
			bool inputClosed, outputClosed;
			bool isBlocking;
		
		public:
			PipeState()
			:
				inputClosed ( false ),
				outputClosed( false ),
				isBlocking  ( true )
			{}
			
			void InputClosed()   { inputClosed  = true; }
			void OutputClosed()  { outputClosed = true; }
			
			bool CloseInput()   { inputClosed  = true;  return outputClosed; }
			bool CloseOutput()  { outputClosed = true;  return inputClosed;  }
			
			bool IsBlocking()  { return isBlocking; }
			void SetBlocking( bool blocking )  { isBlocking = blocking; }
			
			int Read (       char* data, std::size_t byteCount );
			int Write( const char* data, std::size_t byteCount );
	};
	
	class PipeInHandle : public StreamHandle
	{
		private:
			boost::shared_ptr< PipeState > state;
		
		public:
			static TypeCode Type()  { return kPipeType; }
			
			virtual TypeCode ActualType() const  { return Type(); }
			
			PipeInHandle( boost::shared_ptr< PipeState > state ) : state( state )  {}
			
			~PipeInHandle()  { state->CloseInput(); }
			
			unsigned int SysPoll() const  { return kPollWrite; }
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount )
			{
				return state->Write( data, byteCount );
			}
			
			//void IOCtl( unsigned long request, int* argp );
			
			bool IsBlocking() const  { return true; }
			
			void SetBlocking   ()  {}
			void SetNonBlocking()  {}
	};
	
	class PipeOutHandle : public StreamHandle
	{
		private:
			boost::shared_ptr< PipeState > state;
		
		public:
			static TypeCode Type()  { return kPipeType; }
			
			virtual TypeCode ActualType() const  { return Type(); }
			
			PipeOutHandle( boost::shared_ptr< PipeState > state ) : state( state )  {}
			
			~PipeOutHandle()  { state->CloseOutput(); }
			
			unsigned int SysPoll() const  { return kPollRead; }  // FIXME
			
			int SysRead( char* data, std::size_t byteCount )
			{
				return state->Read( data, byteCount );
			}
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			bool IsBlocking() const  { return state->IsBlocking(); }
			
			void SetBlocking   ()  { state->SetBlocking( true  );  }
			void SetNonBlocking()  { state->SetBlocking( false );  }
	};
	
}

#endif

