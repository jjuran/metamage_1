/*	===========
 *	IOStream.hh
 *	===========
 */

#ifndef GENIE_IOSTREAM_HH
#define GENIE_IOSTREAM_HH


namespace Genie
{
	
	enum
	{
		kPollRead   = 1,
		kPollWrite  = 2,
		kPollExcept = 4
	};
	
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
	
}

#endif

