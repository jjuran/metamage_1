/*
	value.hh
	--------
*/

#ifndef VC_VALUE_HH
#define VC_VALUE_HH

// plus
#include "plus/integer.hh"
#include "plus/ref_count.hh"
#include "plus/string.hh"

// vc
#include "vc/expr_box.hh"
#include "vc/function_id.hh"
#include "vc/op_type.hh"
#include "vc/symbol_id.hh"


namespace vc
{
	
	enum value_type
	{
		Value_nothing,
		Value_dummy_operand,
		Value_undefined,
		Value_empty_list,
		Value_symbol_declarator,
		Value_symbol,
		Value_builtin_function,
		Value_boolean,
		Value_number,
		Value_string,
		Value_function,
		Value_pair,
	};
	
	struct Value;
	
	typedef Value (*function_type)( const Value& argument );
	
	struct Value
	{
		value_type     type;
		plus::integer  number;
		plus::string   string;
		function_type  function;
		expr_box       expr;
		
		Value( value_type type = value_type() ) : type( type )
		{
		}
		
		Value( symbol_id sym ) : type( Value_symbol ), number( sym )
		{
		}
		
		Value( function_id f ) : type( Value_builtin_function ), number( f )
		{
		}
		
		Value( bool b ) : type( Value_boolean ), number( b )
		{
		}
		
		Value( unsigned int i ) : type( Value_number ), number( i ) {}
		Value(          int i ) : type( Value_number ), number( i ) {}
		
		Value( unsigned long i ) : type( Value_number ), number( i ) {}
		Value(          long i ) : type( Value_number ), number( i ) {}
		
		Value( unsigned long long i ) : type( Value_number ), number( i ) {}
		Value(          long long i ) : type( Value_number ), number( i ) {}
		
		Value( const plus::integer& i ) : type( Value_number ), number( i )
		{
		}
		
		Value( const plus::string& s ) : type( Value_string ), string( s )
		{
		}
		
		Value( const char* s ) : type( Value_string ), string( s )
		{
		}
		
		Value( function_type f, const plus::string& name )
		:
			type( Value_function ),
			string( name ),
			function( f )
		{
		}
		
		Value( const Value& a, const Value& b );
	};
	
	void swap( Value& a, Value& b );
	
	struct Expr : public plus::ref_count< Expr >
	{
		const op_type  op;
		const Value    left;
		const Value    right;
		
		Expr( const Value& a, op_type op, const Value& b );
	};
	
	inline
	bool is_empty( const Value& v )
	{
		return v.type == Value_empty_list;
	}
	
	inline
	bool is_symbol( const Value& v )
	{
		return v.type == Value_symbol  ||  v.type == Value_symbol_declarator;
	}
	
}

#endif
