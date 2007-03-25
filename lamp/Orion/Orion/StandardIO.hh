/*	=============
 *	StandardIO.hh
 *	=============
 */

#ifndef ORION_STANDARDIO_HH
#define ORION_STANDARDIO_HH

// Standard C/C++
#include <cstring>

// Standard C++
#include <string>

// Nucleus
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/FileDescriptor.hh"


namespace Io
{
	
	template < class T >
	class Stream
	{
		public:
			Stream() : t()  {}
			
			Stream( T t ) : t( t )  {}
			
			T Get() const  { return t; }
		private:
			T t;
	};
	
	template < class T >
	Stream< const T& > S( const T& t )
	{
		return Stream< const T& >( t );
	}
	
	template < class T >
	void Put( const Stream< T >& s, const char* string, std::size_t size )
	{
		int written = io::write( s.Get(), string, size );
		// verify written against size
	}
	
	template < class T >
	void Put( const Stream< T >& s, const std::string& string )
	{
		Put( s, string.data(), string.size() );
	}
	
	template < class T >
	const Stream< T >& operator<<( const Stream< T >& s, const std::string& string )
	{
		Put( s, string );
		return s;
	}
	
	template < class T, class Object >
	const Stream< T >& operator<<( const Stream< T >& s, const Object& object )
	{
		// Covers anything that implicitly converts to a std::string
		// or has a Converter specialized for it
		return s << Nucleus::Convert< std::string >( object );
	}
	
}

namespace Io
{
	
	static Stream< POSeven::FileDescriptor > In ( POSeven::kStdIn_FileNo  ),
	                                         Out( POSeven::kStdOut_FileNo ),
	                                         Err( POSeven::kStdErr_FileNo );
	
}

#endif

