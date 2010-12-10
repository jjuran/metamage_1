/*	==========
 *	Conduit.cc
 *	==========
 */

#include "Genie/IO/Conduit.hh"

// Standard C
#include <errno.h>
#include <signal.h>

// Standard C++
#include <algorithm>

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/api/signals.hh"
#include "Genie/api/yield.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	Page::Page( const Page& other )
	:
		written( other.written ),
		read   ( other.read    )
	{
		ASSERT( read    <= written  );
		ASSERT( written <= capacity );
		
		std::copy( &other.data[ read    ],
		           &other.data[ written ],
		           &      data[ read    ] );
	}
	
	void Page::Write( const char* buffer, std::size_t n_bytes )
	{
		ASSERT( n_bytes <= Writable() );
		
		std::copy( buffer, buffer + n_bytes, &data[ written ] );
		
		written += n_bytes;
	}
	
	std::size_t Page::Read( char* buffer, std::size_t max_bytes )
	{
		max_bytes = std::min( max_bytes, Readable() );
		
		const char* start = &data[ read ];
		
		std::copy( start, start + max_bytes, buffer );
		
		read += max_bytes;
		
		return max_bytes;
	}
	
	
	bool Conduit::IsReadable() const
	{
		return itsIngressHasClosed || !itsPages.empty();
	}
	
	bool Conduit::IsWritable() const
	{
		return itsEgressHasClosed || itsPages.size() < 20;
	}
	
	int Conduit::Read( char* buffer, std::size_t max_bytes, bool nonblocking )
	{
		if ( max_bytes == 0 )
		{
			return 0;
		}
		
		// Wait until we have some data or the stream is closed
		while ( itsPages.empty() && !itsIngressHasClosed )
		{
			try_again( nonblocking );
		}
		
		// Either a page was written, or input was closed,
		// or possibly both, so check itsPages rather than itsIngressHasClosed
		// so we don't miss data.
		
		// If the page queue is still empty then input must have closed.
		if ( itsPages.empty() )
		{
			return 0;
		}
		
		// Only reached if a page is available.
		
		const std::size_t readable = itsPages.front().Readable();
		
		ASSERT( readable > 0 );
		
		const bool consumed = max_bytes >= readable;
		
		itsPages.front().Read( buffer, max_bytes );
		
		if ( consumed )
		{
			itsPages.pop_front();
			
			return readable;
		}
		
		return max_bytes;
	}
	
	int Conduit::Write( const char* buffer, std::size_t n_bytes, bool nonblocking )
	{
		while ( !IsWritable() )
		{
			try_again( nonblocking );
		}
		
		if ( itsEgressHasClosed )
		{
			send_signal_to_current_process( SIGPIPE );
			
			p7::throw_errno( EPIPE );
		}
		
		if ( n_bytes == 0 )
		{
			return 0;
		}
		
		if ( itsPages.empty() )
		{
			itsPages.push_back( Page() );
		}
		else if ( n_bytes > itsPages.back().Writable()  &&  n_bytes <= Page::capacity )
		{
			itsPages.push_back( Page() );
			
			itsPages.back().Write( buffer, n_bytes );
			
			return n_bytes;
		}
		
		const char* end = buffer + n_bytes;
		
		std::size_t writable = 0;
		
		while ( end - buffer > (writable = itsPages.back().Writable()) )
		{
			itsPages.back().Write( buffer, writable );
			
			buffer += writable;
			
			itsPages.push_back( Page() );
		}
		
		itsPages.back().Write( buffer, end - buffer );
		
		
		return n_bytes;
	}
	
}

