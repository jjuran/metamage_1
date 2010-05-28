/*	==========
 *	Conduit.hh
 *	==========
 */

#ifndef GENIE_IO_CONDUIT_HH
#define GENIE_IO_CONDUIT_HH

// Standard C++
#include <list>

// plus
#include "plus/ref_count.hh"


namespace Genie
{
	
	class Page
	{
		public:
			static const std::size_t capacity = 4096;
		
		private:
			std::size_t written;
			std::size_t read;
			
			char data[ capacity ];
		
		public:
			Page() : written(), read()
			{
			}
			
			Page( const Page& other );
			
			std::size_t Writable() const  { return capacity - written; }
			std::size_t Readable() const  { return written  - read;    }
			
			bool Whole() const  { return read == 0  &&  written == capacity; }
			
			void Write( const char* buffer, std::size_t n_bytes );
			
			std::size_t Read ( char* buffer, std::size_t max_bytes );
	};
	
	class Conduit : public plus::ref_count< Conduit >
	{
		private:
			std::list< Page > itsPages;
			
			bool itsIngressHasClosed;
			bool itsEgressHasClosed;
		
		public:
			Conduit() : itsIngressHasClosed ( false ),
			            itsEgressHasClosed( false )
			{
			}
			
			bool IsReadable() const;
			bool IsWritable() const;
			
			bool IngressHasClosed() const  { return itsIngressHasClosed; }
			bool EgressHasClosed()  const  { return itsEgressHasClosed;  }
			
			bool CloseIngress()  { itsIngressHasClosed = true;  return itsEgressHasClosed;  }
			bool CloseEgress()   { itsEgressHasClosed  = true;  return itsIngressHasClosed; }
			
			int Read (       char* data, std::size_t byteCount, bool nonblocking );
			int Write( const char* data, std::size_t byteCount, bool nonblocking );
	};
	
}

#endif

