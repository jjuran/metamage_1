/*
	file_descriptor.cc
	------------------
*/

#include "varyx/posix/file_descriptor.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <errno.h>

// Standard C++
#include <new>

// more-libc
#include "more/string.h"

// gear
#include "gear/inscribe_decimal.hh"

// vlib
#include "vlib/generic.hh"
#include "vlib/list-utils.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/type.hh"

// varyx-posix
#include "varyx/posix/posixfs.hh"


#define STRLEN( s )  (sizeof "" s - 1)


namespace varyx
{
namespace posix
{
	
	using namespace vlib;
	
	
	inline
	auto_fd::~auto_fd()
	{
		if ( ! it_is_closed )
		{
			::close( its_fd );
		}
	}
	
	
	Value FileDescriptor::coerce( const Value& v )
	{
		int fd = integer_cast< int >( v );
		
		return FileDescriptor( fd );
	}
	
	static
	size_t fd_str_size( const Value& v )
	{
		const FileDescriptor& that = static_cast< const FileDescriptor& >( v );
		
		return gear::decimal_magnitude( that.get() );
	}
	
	static
	char* fd_str_copy( char* p, const Value& v )
	{
		const FileDescriptor& that = static_cast< const FileDescriptor& >( v );
		
		return gear::inscribe_decimal_r( that.get(), p );
	}
	
	static const stringify fd_str =
	{
		NULL,
		&fd_str_size,
		&fd_str_copy,
	};
	
	static
	size_t fd_rep_size( const Value& v )
	{
		const FileDescriptor& that = static_cast< const FileDescriptor& >( v );
		
		size_t wrapper_size = STRLEN( "(fd )" );
		
		if ( that.is_automatic() )
		{
			wrapper_size += STRLEN( "auto " );
		}
		
		return wrapper_size + gear::decimal_magnitude( that.get() );
	}
	
	static
	char* fd_rep_copy( char* p, const Value& v )
	{
		const FileDescriptor& that = static_cast< const FileDescriptor& >( v );
		
		*p++ = '(';
		
		if ( that.is_automatic() )
		{
			*p++ = 'a';
			*p++ = 'u';
			*p++ = 't';
			*p++ = 'o';
			*p++ = ' ';
		}
		
		*p++ = 'f';
		*p++ = 'd';
		*p++ = ' ';
		
		p = gear::inscribe_decimal_r( that.get(), p );
		
		*p++ = ')';
		
		return p;
	}
	
	static const stringify fd_rep =
	{
		NULL,
		&fd_rep_size,
		&fd_rep_copy,
	};
	
	static
	size_t fd_bin_size( const Value& v )
	{
		return sizeof (int);
	}
	
	static
	char* fd_bin_copy( char* p, const Value& v )
	{
		const FileDescriptor& that = static_cast< const FileDescriptor& >( v );
		
		int fd = that.get();
		
		return (char*) mempcpy( p, &fd, sizeof fd );
	}
	
	static const stringify fd_bin =
	{
		NULL,
		&fd_bin_size,
		&fd_bin_copy,
	};
	
	const stringifiers fd_stringifiers =
	{
		&fd_str,
		&fd_rep,
		&fd_bin,
	};
	
	static
	bool non_negative( const Value& v )
	{
		const FileDescriptor& that = static_cast< const FileDescriptor& >( v );
		
		return that.get() >= 0;  // fd 0 is true, fd -1 is false
	}
	
	static const veritization fd_veritization =
	{
		&non_negative,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		const FileDescriptor& that = static_cast< const FileDescriptor& >( v );
		
		switch ( op )
		{
			case Op_typeof:
				return Type( fd_vtype );
			
			case Op_unary_deref:
				return Integer( that.get() );
			
			case Op_auto:
				if ( that.is_automatic() )
				{
					return v;
				}
				
				return FileDescriptor( that.get(), automatic );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		const FileDescriptor& that = static_cast< const FileDescriptor& >( a );
		
		switch ( op )
		{
			case Op_send:
				send_data( that.get(), b );
				return a;
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
	};
	
	const dispatch fd_dispatch =
	{
		&fd_stringifiers,
		&fd_veritization,
		NULL,
		&ops,
	};
	
	FileDescriptor::FileDescriptor( int fd, automatic_t )
	:
		Value( sizeof (auto_fd),
		       &generic_destructor< auto_fd >,
		       Value_other,
		       &fd_dispatch )
	{
		new ((void*) pointer()) auto_fd( fd );
	}
	
	int FileDescriptor::close() const
	{
		if ( is_automatic() )
		{
			const auto_fd& autofd = dereference< auto_fd >();
			
			if ( autofd.closed() )
			{
				// Don't close the same fd twice
				errno = EBADF;
				return -1;
			}
			
			const_cast< auto_fd& >( autofd ).closing();
		}
		
		return ::close( get() );
	}
	
	static
	Value fd_member( const Value& obj, const plus::string& member )
	{
		if ( member == "read" )
		{
			return bind_args( Proc( proc_read ), obj );
		}
		
		THROW( "nonexistent fd member" );
		
		return Value_nothing;
	}
	
	const type_info fd_vtype =
	{
		"fd",
		&assign_to< FileDescriptor >,
		&FileDescriptor::coerce,
		NULL,
		&fd_member,
		Type_pure,
	};
	
}
}
