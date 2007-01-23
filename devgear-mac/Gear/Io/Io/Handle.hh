/*	=========
 *	Handle.hh
 *	=========
 */

#ifndef IO_HANDLE_HH
#define IO_HANDLE_HH

// Nucleus
#include "Nucleus/Shared.h"


namespace Io
{
	
	namespace NN = Nucleus;
	
	inline void SetBlockingNoOp( void*, bool )
	{
	}
	
	class Handle
	{
		typedef void ( *BlockingSetter )( void* object, bool blocking );
		typedef int  ( *Reader         )( void* object,       char*, std::size_t );
		typedef int  ( *Writer         )( void* object, const char*, std::size_t );
		typedef void ( *Deleter        )( void* object );
		
		protected:  // SocketHandle needs access to object
			
			class Disposer
			{
				private:
					Deleter deleter;
				
				public:
					Disposer() : deleter( NULL )  {}
					
					Disposer( Deleter deleter ) : deleter( deleter )  {}
					
					void operator()( void* object ) const  { deleter( object ); }
			};
			
			NN::Shared< void*, Disposer > object;
			BlockingSetter blockingSetter;
			Reader reader;
			Writer writer;
		
		public:
			Handle()
			:
				object(),
				blockingSetter( NULL ),
				reader( NULL ),
				writer( NULL )
			{}
			
			Handle( void*           object,
			        BlockingSetter  blockingSetter,
			        Reader          reader,
			        Writer          writer,
			        Deleter         deleter )
			:
				object( NN::Owned< void*, Disposer >::Seize( object, deleter ) ),
				blockingSetter( blockingSetter ),
				reader( reader ),
				writer( writer )
			{}
			
			friend bool operator==( const Handle& a, const Handle& b )
			{
				return a.object.Get() == b.object.Get();
			}
			
			friend bool operator!=( const Handle& a, const Handle& b )
			{
				return !( a == b );
			}
			
			void SetBlocking( bool blocking ) const  { return blockingSetter( object.Get(), blocking ); }
			
			int Read (       char* data, std::size_t byteCount ) const  { return reader( object.Get(), data, byteCount ); }
			int Write( const char* data, std::size_t byteCount ) const  { return writer( object.Get(), data, byteCount ); }
	};
	
	typedef Handle IOHandle;
	
	inline int Read( const Handle& h, char* data, std::size_t byteCount )
	{
		return h.Read( data, byteCount );
	}
	
	inline int Write( const Handle& h, const char* data, std::size_t byteCount )
	{
		return h.Write( data, byteCount );
	}
	
}

namespace io
{
	
	inline int read( Io::Handle h, char* buffer, std::size_t byteCount )
	{
		return Io::Read( h, buffer, byteCount );
	}
	
	inline int write( Io::Handle h, const char* buffer, std::size_t byteCount )
	{
		return Io::Write( h, buffer, byteCount );
	}
	
}

#endif

