/*	===========
 *	IOHandle.hh
 *	===========
 */

#ifndef GENIE_IOHANDLE_HH
#define GENIE_IOHANDLE_HH

// Io
#include "Io/Handle.hh"

// Genie
#include "Genie/IORef.hh"


namespace Genie
{
	
	class IOHandle : public IOStream
	{
		private:
			Io::Handle handle;
			bool isBlocking;
		
		public:
			IOHandle( const Io::Handle& handle ) : handle( handle ), isBlocking( true )  {}
			
			unsigned int SysPoll() const  { return kPollRead | kPollWrite; }
			
			int SysRead( char* data, std::size_t byteCount )
			{
				return handle.Read( data, byteCount );
			}
			
			int SysWrite( const char* data, std::size_t byteCount )
			{
				return handle.Write( data, byteCount );
			}
			
			bool IsBlocking() const  { return isBlocking; }
			
			void SetBlocking   ();
			void SetNonBlocking();
	};
	
	IORef NewGenericIO( const Io::Handle& io );
	
}

#endif

