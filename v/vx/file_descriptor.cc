/*
	file_descriptor.cc
	------------------
*/

#include "file_descriptor.hh"

// more-libc
#include "more/string.h"

// gear
#include "gear/inscribe_decimal.hh"

// vlib
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/type.hh"

// vx
#include "posixfs.hh"


namespace vlib
{
	
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
		
		return 5 + gear::decimal_magnitude( that.get() );
	}
	
	static
	char* fd_rep_copy( char* p, const Value& v )
	{
		const FileDescriptor& that = static_cast< const FileDescriptor& >( v );
		
		*p++ = '(';
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
	
	const type_info fd_vtype =
	{
		"fd",
		&assign_to< FileDescriptor >,
		&FileDescriptor::coerce,
		NULL,
	};
	
}
