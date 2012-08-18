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
	
	class page
	{
		public:
			static const std::size_t capacity = 4096;
		
		private:
			std::size_t n_written;
			std::size_t n_read;
			
			char data[ capacity ];
		
		public:
			page() : n_written(), n_read()
			{
			}
			
			page( const page& other );
			
			std::size_t n_writable() const  { return capacity  - n_written; }
			std::size_t n_readable() const  { return n_written - n_read;    }
			
			bool whole() const  { return n_read == 0  &&  n_written == capacity; }
			
			void write( const char* buffer, std::size_t n_bytes );
			
			std::size_t read( char* buffer, std::size_t max_bytes );
	};
	
	class Conduit : public plus::ref_count< Conduit >
	{
		private:
			std::list< page > its_pages;
			
			bool its_ingress_has_closed;
			bool its_egress_has_closed;
		
		public:
			Conduit() : its_ingress_has_closed( false ),
			            its_egress_has_closed ( false )
			{
			}
			
			bool IsReadable() const;
			bool IsWritable() const;
			
			bool IngressHasClosed() const  { return its_ingress_has_closed; }
			bool EgressHasClosed()  const  { return its_egress_has_closed;  }
			
			bool CloseIngress()  { its_ingress_has_closed = true;  return its_egress_has_closed;  }
			bool CloseEgress()   { its_egress_has_closed  = true;  return its_ingress_has_closed; }
			
			int Read (       char* data, std::size_t byteCount, bool nonblocking );
			int Write( const char* data, std::size_t byteCount, bool nonblocking );
	};
	
}

#endif

