/*
	vector.cc
	---------
*/

#include "vlib/types/vector/vector.hh"

// Standard C
#include <string.h>

// vlib
#include "vlib/function-utils.hh"
#include "vlib/target.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"
#include "vlib/types/vector/type.hh"


namespace vlib
{
	
	static
	const char* vector_str_data( const Value& v )
	{
		return "<vector>";
	}
	
	static const stringify vector_str =
	{
		&vector_str_data,
		NULL,
		NULL,
	};
	
	static
	const char* vector_bin_data( const Value& v )
	{
		const Vector& vector = static_cast< const Vector& >( v );
		
		const plus::string& memory = vector.expr()->right.string();
		
		return memory.data();
	}
	
	static
	size_t vector_bin_size( const Value& v )
	{
		const Vector& vector = static_cast< const Vector& >( v );
		
		const plus::string& memory = vector.expr()->right.string();
		
		return memory.size();
	}
	
	static const stringify vector_bin =
	{
		&vector_bin_data,
		&vector_bin_size,
		NULL,
	};
	
	static const stringifiers vector_stringifiers =
	{
		&vector_str,
		NULL,
		&vector_bin,
	};
	
	static
	bool nonempty_vector( const Value& v )
	{
		const plus::string& buffer = v.expr()->right.string();
		
		return ! buffer.empty();
	}
	
	static const veritization vector_veritization =
	{
		&nonempty_vector,
	};
	
	static
	Value element_list( const Value& v )
	{
		list_builder list;
		
		const Vector& vector = static_cast< const Vector& >( v );
		
		const size_t n = vector.size();
		
		for ( size_t i = 0;  i < n;  ++i )
		{
			list.append( vector.at( i ) );
		}
		
		return list;
	}
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return v.expr()->left;
			
			case Op_unary_deref:
				return element_list( v );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		const Vector& vector = static_cast< const Vector& >( a );
		
		switch ( op )
		{
			case Op_subscript:
				return vector.at( integer_cast< size_t >( b ) );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value mutating_op_handler( op_type        op,
	                           const Target&  target,
	                           const Value&   x,
	                           const Value&   b )
	{
		Vector& vector = static_cast< Vector& >( *target.addr );
		
		const size_t i = integer_cast< size_t >( x.expr()->right );
		
		switch ( op )
		{
			case Op_duplicate:
			case Op_approximate:
				return vector.assign_at( b, i, op == Op_approximate );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
		NULL,
		&mutating_op_handler,
	};
	
	const dispatch vector_dispatch =
	{
		&vector_stringifiers,
		&vector_veritization,
		NULL,
		&ops,
	};
	
	static const String size_name( "size" );
	
	static const String encode_name( "encode" );
	static const String decode_name( "decode" );
	
	static
	handler_2arg get_binary_ops_handler( const Value& object )
	{
		if ( const dispatch* methods = object.dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_2arg handler = ops->binary )
				{
					return handler;
				}
			}
		}
		
		THROW( "invalid encoder/decoder object in vector type" );
		
		return NULL;
	}
	
	static inline
	handler_2arg get_binary_ops_handler_unchecked( const Value& object )
	{
		return object.dispatch_methods()->ops->binary;
	}
	
	static
	Value member_op( handler_2arg handler, const Value& obj, const Value& name )
	{
		const Value result = handler( Op_member, obj, name );
		
		if ( result.type() )
		{
			return result;
		}
		
		THROW( "required endec member missing" );
		
		return NIL;
	}
	
	static inline
	size_t endec_unit_size( handler_2arg handler, const Value& endec )
	{
		const Value unit_size = member_op( handler, endec, size_name );
		
		/*
			unit_size will be NIL if the member doesn't exist.  Or it might
			be defined as `null` for variable-width endecs.  In any case, we
			require a fixed width for vectors, so reject anything else.
		*/
		
		return integer_cast< size_t >( unit_size );
	}
	
	static
	size_t endec_unit_size( const Value& endec )
	{
		handler_2arg handler = get_binary_ops_handler( endec );
		
		size_t unit_size = endec_unit_size( handler, endec );
		
		if ( unit_size == 0 )
		{
			THROW( "vector unit size can't be zero" );
		}
		
		return unit_size;
	}
	
	static inline
	size_t endec_unit_size_unchecked( const Value& endec )
	{
		handler_2arg handler = get_binary_ops_handler_unchecked( endec );
		
		return endec_unit_size( handler, endec );
	}
	
	static inline
	Value endec_encode( handler_2arg handler, const Value& endec )
	{
		return member_op( handler, endec, encode_name );
	}
	
	static inline
	Value endec_decode( handler_2arg handler, const Value& endec )
	{
		return member_op( handler, endec, decode_name );
	}
	
	static inline
	const Value& endec_from_type( const Value& type )
	{
		const Vector_Type& vt = static_cast< const Vector_Type& >( type );
		
		return vt.element_type();
	}
	
	static inline
	size_t sizeof_vector( const Value& type, size_t n )
	{
		const size_t unit_size = endec_unit_size( endec_from_type( type ) );
		
		if ( n > (size_t) -1 / unit_size )
		{
			THROW( "vector size overflow" );
		}
		
		return unit_size * n;
	}
	
	Vector::Vector( const Value& type, size_t n )
	:
		Value( type,
		       Op_list,
		       Packed( plus::string( sizeof_vector( type, n ), '\0' ) ),
		       &vector_dispatch )
	{
	}
	
	Vector::Vector( const Value& type, const plus::string& buffer )
	:
		Value( type,
		       Op_list,
		       Packed( buffer ),
		       &vector_dispatch )
	{
		const size_t unit_size = endec_unit_size( endec_from_type( type ) );
		
		if ( buffer.size() % unit_size )
		{
			THROW( "vector input buffer size not a multiple of unit width" );
		}
	}
	
	const Value& Vector::get_endec() const
	{
		return endec_from_type( get_type() );
	}
	
	size_t Vector::size() const
	{
		const size_t unit_size = endec_unit_size_unchecked( get_endec() );
		
		return expr()->right.string().size() / unit_size;
	}
	
	Value Vector::at( size_t i ) const
	{
		const Value& endec = get_endec();
		
		handler_2arg handler = get_binary_ops_handler_unchecked( endec );
		
		const size_t unit_size = endec_unit_size( handler, endec );
		
		const plus::string& memory = expr()->right.string();
		
		const size_t length = memory.size() / unit_size;
		
		if ( i >= length )
		{
			THROW( "vector subscript out of bounds" );
		}
		
		plus::string unit = memory.substr( i * unit_size, unit_size );
		
		const Value decode = endec_decode( handler, endec );
		
		const Value result = call_function( decode, Packed( unit ) );
		
		return result;
	}
	
	Value Vector::assign_at( const Value& v, size_t i, bool coercive )
	{
		// TODO:  What about coercion?
		
		const Value& endec = get_endec();
		
		handler_2arg handler = get_binary_ops_handler_unchecked( endec );
		
		const size_t unit_size = endec_unit_size( handler, endec );
		
		const plus::string& memory = expr()->right.unshare().string();
		
		const size_t length = memory.size() / unit_size;
		
		if ( i >= length )
		{
			THROW( "vector subscript out of bounds" );
		}
		
		const Value encode = endec_encode( handler, endec );
		
		const Value encoded = call_function( encode, v );
		
		if ( const Packed* packed = encoded.is< Packed >() )
		{
			const plus::string& pack = packed->string();
			
			if ( pack.size() != unit_size )
			{
				THROW( "vector encoder result is the wrong size" );
			}
			
			char* data = (char*) memory.data() + i * unit_size;
			
			memcpy( data, pack.data(), unit_size );
		}
		else
		{
			THROW( "vector encoder result is not packed data" );
		}
		
		return v;
	}
	
	static
	Value vector_member( const Value& obj,
	                     const plus::string& member )
	{
		const Vector& vector = static_cast< const Vector& >( obj );
		
		if ( member == "length" )
		{
			return Integer( vector.size() );
		}
		
		THROW( "nonexistent vector member" );
		
		return Value();
	}
	
	const type_info vector_vtype =
	{
		"vector",
		&assign_to< Vector >,
		NULL,
		&vector_member,
	};
	
}
