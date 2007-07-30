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
			bool itsInputHasClosed, itsOutputHasClosed;
			bool itIsBlocking;
		
		public:
			PipeState()
			:
				itsInputHasClosed ( false ),
				itsOutputHasClosed( false ),
				itIsBlocking      ( true )
			{}
			
			void InputClosed()   { itsInputHasClosed  = true; }
			void OutputClosed()  { itsOutputHasClosed = true; }
			
			bool CloseInput()   { itsInputHasClosed  = true;  return itsOutputHasClosed; }
			bool CloseOutput()  { itsOutputHasClosed = true;  return itsInputHasClosed;  }
			
			bool IsBlocking()  { return itIsBlocking; }
			void SetBlocking( bool blocking )  { itIsBlocking = blocking; }
			
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
			
			~PipeInHandle();
			
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
			
			~PipeOutHandle();
			
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

