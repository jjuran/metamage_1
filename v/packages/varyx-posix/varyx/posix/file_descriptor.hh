/*
	file_descriptor.hh
	------------------
*/

#ifndef VARYX_POSIX_FILEDESCRIPTOR_HH
#define VARYX_POSIX_FILEDESCRIPTOR_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
}

namespace varyx
{
namespace posix
{
	
	using vlib::dispatch;
	using vlib::type_info;
	using vlib::Value;
	using vlib::Value_other;
	
	extern const dispatch fd_dispatch;
	
	enum automatic_t { automatic };
	
	class auto_fd
	{
		private:
			int its_fd;
			bool it_is_closed;
			
			// non-copyable
			auto_fd           ( const auto_fd& );
			auto_fd& operator=( const auto_fd& );
		
		public:
			auto_fd( int fd ) : its_fd( fd ), it_is_closed()
			{
			}
			
			~auto_fd();
			
			int get() const  { return its_fd; }
			
			bool closed() const  { return it_is_closed; }
			void closing()       { it_is_closed = true; }
	};
	
	class FileDescriptor : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &fd_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			FileDescriptor( int fd ) : Value( Value_other, &fd_dispatch )
			{
				pod_cast< int >() = fd;
			}
			
			FileDescriptor( int fd, automatic_t );
			
			bool is_automatic() const
			{
				return has_extent()  &&  ! dereference< auto_fd >().closed();
			}
			
			int get() const
			{
				if ( has_extent() )
				{
					return dereference< auto_fd >().get();
				}
				
				return pod_cast< int >();
			}
			
			int close() const;
	};
	
	extern const type_info fd_vtype;
	
}
}

#endif
