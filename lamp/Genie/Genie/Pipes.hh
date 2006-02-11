/*	========
 *	Pipes.hh
 *	========
 *	
 *	Implemented by Pipes.cc
 */

// Standard C++
#include <list>
#include <string>

// Nitrogen Nucleus
#include "Nucleus/Shared.h"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	class PipeState
	{
		private:
			std::list< std::string > myStrings;
			std::string partialRead;
			bool inputClosed, outputClosed;
			bool fBlocking;
		
		public:
			PipeState()
			:
				inputClosed ( false ),
				outputClosed( false ),
				fBlocking   ( true )
			{}
			
			void InputClosed()   { inputClosed  = true; }
			void OutputClosed()  { outputClosed = true; }
			
			void SetBlocking( bool blocking )  { fBlocking = blocking; }
			
			int Read (       char* data, std::size_t byteCount );
			int Write( const char* data, std::size_t byteCount );
	};
	
	struct CloseInput
	{
		void operator()( PipeState* state ) const
		{
			if ( state != NULL )
			{
				state->InputClosed();
			}
		}
	};
	
	struct CloseOutput
	{
		void operator()( PipeState* state ) const
		{
			if ( state != NULL )
			{
				state->OutputClosed();
			}
		}
	};
	
}

namespace Nucleus
{
	
	template <>  struct Disposer< Genie::PipeState* > : DisposeWithDelete  {};
	
}

namespace Genie
{
	
	class PipeOutHandle
	{
		private:
			NN::Shared< PipeState*              > pipe;
			NN::Shared< PipeState*, CloseOutput > output;
		
		public:
			PipeOutHandle()  {}
			
			PipeOutHandle( NN::Shared< PipeState*              > pipe, 
			               NN::Shared< PipeState*, CloseOutput > output )
			:
				pipe  ( pipe   ), 
				output( output )
			{}
			
			void SetBlocking( bool blocking ) const  { pipe.Get()->SetBlocking( blocking ); }
			
			int Read (       char* data, std::size_t byteCount ) const;
			int Write( const char* data, std::size_t byteCount ) const;
	};
	
	class PipeInHandle
	{
		private:
			NN::Shared< PipeState*             > pipe;
			NN::Shared< PipeState*, CloseInput > input;
		
		public:
			PipeInHandle()  {}
			
			PipeInHandle( NN::Shared< PipeState*             > pipe,
			              NN::Shared< PipeState*, CloseInput > input )
			:
				pipe ( pipe  ), 
				input( input )
			{}
			
			int Read (       char* data, std::size_t byteCount ) const;
			int Write( const char* data, std::size_t byteCount ) const;
	};
	
	inline int Read( const PipeOutHandle& out, char* data, std::size_t byteCount )
	{
		return out.Read( data, byteCount );
	}
	
	inline int Write( const PipeOutHandle& out, const char* data, std::size_t byteCount )
	{
		return out.Write( data, byteCount );
	}
	
	inline int Read( const PipeInHandle& in, char* data, std::size_t byteCount )
	{
		return in.Read( data, byteCount );
	}
	
	inline int Write( const PipeInHandle& in, const char* data, std::size_t byteCount )
	{
		return in.Write( data, byteCount );
	}
	
	class Pipe : public PipeState
	{
		private:
			Pipe();
		
		public:
			struct Handles
			{
				Handles()  {}
				
				Handles( PipeInHandle in, PipeOutHandle out )
				:
					in ( in  ),
					out( out )
				{}
				
				PipeInHandle  in;
				PipeOutHandle out;
			};
			
			static Handles New();
	};
	
	struct NotReadable   {};
	struct NotWriteable  {};
	
	struct PipeIn_IODetails
	{
		static void SetBlocking( const PipeInHandle& in, bool blocking )
		{
		}
		
		static int Read ( const PipeInHandle& in,       char* data, std::size_t byteCount )
		{
			return in.Read( data, byteCount );
		}
		
		static int Write( const PipeInHandle& in, const char* data, std::size_t byteCount )
		{
			return in.Write( data, byteCount );
		}
	};
	
	struct PipeOut_IODetails
	{
		static void SetBlocking( const PipeOutHandle& out, bool blocking )
		{
			out.SetBlocking( blocking );
		}
		
		static int Read ( const PipeOutHandle& out,       char* data, std::size_t byteCount )
		{
			return out.Read( data, byteCount );
		}
		
		static int Write( const PipeOutHandle& out, const char* data, std::size_t byteCount )
		{
			return out.Write( data, byteCount );
		}
	};
	
}

namespace Io
{
	
	using Genie::Read;
	using Genie::Write;
	
}

