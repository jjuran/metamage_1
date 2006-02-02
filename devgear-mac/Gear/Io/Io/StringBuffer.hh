/*	===============
 *	StringBuffer.hh
 *	===============
 */

#ifndef IO_STRINGBUFFER_HH
#define IO_STRINGBUFFER_HH

// Standard C++
#include <string>


namespace Io
{
	
	class StringBuffer
	{
		private:
			std::string fData;
		
		public:
			bool Empty() const  { return fData.empty(); }
			
			int Read( char* data, std::size_t byteCount );
			
			int Write( const char* data, std::size_t byteCount );
	};
	
}

#endif

