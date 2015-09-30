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
		Value_boolean,
		Value_number,
		Value_string,
		Value_pair,
	};
	
	inline
	bool is_symbol( value_type type )
	{
		return type == Value_symbol  ||  type == Value_symbol_declarator;
	}
	
	struct Value
	{
		value_type     type;
		plus::integer  number;
		plus::string   string;
		expr_box       expr;
		
		Value( value_type type = value_type() ) : type( type )
		{
		}
		
		Value( symbol_id sym ) : type( Value_symbol ), number( sym )
		{
		}
		
		Value( bool b ) : type( Value_boolean ), number( b )
		{
		}
		
		Value( const plus::integer& i ) : type( Value_number ), number( i )
		{
		}
		
		Value( const plus::string& s ) : type( Value_string ), string( s )
		{
		}
		
		Value( const char* s ) : type( Value_string ), string( s )
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
	
}

#endif
