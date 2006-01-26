/*	========
 *	IORef.hh
 *	========
 */

#ifndef GENIE_IOREF_HH
#define GENIE_IOREF_HH

// Genie
#include "Genie/ResourceTable.hh"  // Needed for IORef_Cast


namespace Genie
{
	
	enum
	{
		// Quick temporary hack
		kUnknownIODescriptor       = 0,
		kGenericIODescriptor       = 1,
		kBlockDeviceDescriptor     = 2,
		kCharacterDeviceDescriptor = 3,
		kFileDescriptor            = 4,
		kTerminalDescriptor        = 5,
		kSocketDescriptor          = 6
	};
	
	enum
	{
		kPollRead   = 1,
		kPollWrite  = 2,
		kPollExcept = 4
	};
	
	typedef std::size_t (*RefMod)( std::size_t offset, int delta );
	typedef unsigned int (*IOPoller)( std::size_t offset );
	typedef int (*IOReader)( std::size_t offset, char* data, std::size_t byteCount );
	typedef int (*IOWriter)( std::size_t offset, const char* data, std::size_t byteCount );
	
	void RetainIO ( std::size_t type, std::size_t offset );
	void ReleaseIO( std::size_t type, std::size_t offset );
	
	unsigned int PollIO( std::size_t type, std::size_t offset );
	
	int ReadIO ( std::size_t type, std::size_t offset,       char* data, std::size_t byteCount );
	int WriteIO( std::size_t type, std::size_t offset, const char* data, std::size_t byteCount );
	
	void RegisterIOType( std::size_t type,
	                     RefMod refMod,
	                     IOReader reader,
	                     IOWriter writer,
	                     IOPoller poller );
	
	
	class IOStream
	{
		private:
			std::string fPeekBuffer;
		
		public:
			
			virtual unsigned int SysPoll() const = 0;
			
			virtual int SysRead( char* data, std::size_t byteCount ) = 0;
			
			virtual int SysWrite( const char* data, std::size_t byteCount ) = 0;
			
			virtual bool IsBlocking() const = 0;
			
			virtual void SetBlocking   () = 0;
			virtual void SetNonBlocking() = 0;
			
			const std::string& Peek( ByteCount minBytes );
			
			unsigned int Poll() const;
			
			int Read( char* data, std::size_t byteCount );
			
			int Write( const char* data, std::size_t byteCount );
	};
	
	class IORef
	{
		private:
			std::size_t fType;    // selector, range of values modifiable at runtime
			std::size_t fOffset;  // offset into type-specific filetable
			
			IOStream* fStream;
		
		public:
			// Default constructor
			IORef() : fType( 0 ), fOffset( 0 ), fStream( NULL )  {}
			
			// Construct from components
			IORef( std::size_t type, std::size_t offset, IOStream* stream )
			:
				fType  ( type   ),
				fOffset( offset ),
				fStream( stream )
			{
				RetainIO( fType, fOffset );
			}
			
			// Copy constructor
			IORef( const IORef& ref )
			:
				fType  ( ref.fType   ),
				fOffset( ref.fOffset ),
				fStream( ref.fStream )
			{
				RetainIO( fType, fOffset );
			}
			
			// Destructor
			~IORef()
			{
				ReleaseIO( fType, fOffset );
			}
			
			// Copy assignment
			IORef& operator=( const IORef& ref )
			{
				// Retain first in case of self-assignment
				RetainIO( ref.fType, ref.fOffset );
				
				// Now release
				ReleaseIO( fType, fOffset );
				
				// And copy
				fType   = ref.fType;
				fOffset = ref.fOffset;
				fStream = ref.fStream;
				
				return *this;
			}
			
			void Swap( IORef& ref )
			{
				std::swap( fType,   ref.fType   );
				std::swap( fOffset, ref.fOffset );
				std::swap( fStream, ref.fStream );
			}
			
			std::size_t Offset() const  { return fOffset; }
			
			bool IsType( int type ) const
			{
				return fType == type;
			}
			
			const std::string& Peek( ByteCount minBytes ) const;
			
			unsigned int Poll() const;
			
			int Read( char* data, std::size_t byteCount ) const;
			
			int Write( const char* data, std::size_t byteCount ) const;
			
			bool IsBlocking() const;
			void SetBlocking();
			void SetNonBlocking();
	};
	
	template < class Resource >
	Resource& IORef_Cast( const IORef& ref )
	{
		return ResourceTable< Resource >::Lookup( ref.Offset() );
	}
	
}

namespace Io
{
	
	static int Read( const Genie::IORef& handle, char* data, std::size_t byteCount )
	{
		return handle.Read( data, byteCount );
	}
	
	static int Write( const Genie::IORef& handle, const char* data, std::size_t byteCount )
	{
		return handle.Write( data, byteCount );
	}
	
}

#endif

