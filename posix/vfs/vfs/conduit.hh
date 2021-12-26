/*
	conduit.hh
	----------
*/

#ifndef VFS_CONDUIT_HH
#define VFS_CONDUIT_HH

// Standard C++
#include <list>

// vxo
#include "vxo/ref_count.hh"


namespace vfs
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
	
	class conduit : public vxo::ref_count< conduit >
	{
		private:
			typedef void (*try_again_f)( bool );
			typedef void (*broken_pipe_f)();
			
			std::list< page > its_pages;
			
			bool its_ingress_has_closed;
			bool its_egress_has_closed;
		
		public:
			conduit() : its_ingress_has_closed( false ),
			            its_egress_has_closed ( false )
			{
			}
			
			std::size_t pages_used() const  { return its_pages.size(); }
			
			bool is_readable() const;
			bool is_writable() const;
			
			bool ingress_has_closed() const  { return its_ingress_has_closed; }
			bool egress_has_closed()  const  { return its_egress_has_closed;  }
			
			bool close_ingress()  { its_ingress_has_closed = true;  return its_egress_has_closed;  }
			bool close_egress()   { its_egress_has_closed  = true;  return its_ingress_has_closed; }
			
			int read (       char* data, std::size_t n, bool nonblocking, try_again_f                );
			int write( const char* data, std::size_t n, bool nonblocking, try_again_f, broken_pipe_f );
	};
	
}

#endif
