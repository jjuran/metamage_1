/*
	file_descriptor.hh
	------------------
*/

#ifndef FILEDESCRIPTOR_HH
#define FILEDESCRIPTOR_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch fd_dispatch;
	
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
			
			int get() const
			{
				return pod_cast< int >();
			}
	};
	
	extern const type_info fd_vtype;
	
}

#endif
