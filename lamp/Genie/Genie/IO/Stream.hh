/*	=========
 *	Stream.hh
 *	=========
 */

#ifndef GENIE_IO_STREAM_HH
#define GENIE_IO_STREAM_HH

// Standard C++
#include <string>

// Genie
#include "Genie/FileSystem/FSTree.hh"
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	class StreamHandle : public IOHandle
	{
		private:
			std::string peekBuffer;
		
		public:
			virtual ~StreamHandle()  {}
			
			static TypeCode Type()  { return kStreamType; }
			
			virtual TypeCode ActualType() const  { return Type(); }
			
			virtual unsigned int SysPoll() const = 0;
			
			virtual int SysRead( char* data, std::size_t byteCount ) = 0;
			
			virtual int SysWrite( const char* data, std::size_t byteCount ) = 0;
			
			virtual void IOCtl( unsigned long request, int* argp );
			
			virtual bool IsBlocking() const = 0;
			
			virtual void SetBlocking   () = 0;
			virtual void SetNonBlocking() = 0;
			
			const std::string& Peek( ByteCount minBytes );
			
			unsigned int Poll() const;
			
			int Read( char* data, std::size_t byteCount );
			
			int Write( const char* data, std::size_t byteCount );
	};
	
}

#endif

