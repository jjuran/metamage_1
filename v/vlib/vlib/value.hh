/*
	value.hh
	--------
*/

#ifndef VLIB_VALUE_HH
#define VLIB_VALUE_HH

// plus
#include "plus/integer.hh"
#include "plus/ref_count.hh"
#include "plus/string.hh"

// vlib
#include "vlib/expr_box.hh"
#include "vlib/op_type.hh"
#include "vlib/symbol_id.hh"


namespace vlib
{
	
	struct proc_info;
	
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
		Value_function,
		Value_pair,
		
		V_decl = Value_symbol_declarator,
		V_bool = Value_boolean,
		V_int  = Value_number,
		V_str  = Value_string,
		V_proc = Value_function,
	};
	
	typedef const proc_info* proc_t;
	
	struct Value
	{
		public:
			value_type     type;
			plus::integer  number;
			plus::string   string;
			proc_t         function;
			expr_box       expr;
		
		public:
			Value( value_type type = value_type() ) : type( type )
			{
			}
		
			Value( symbol_id sym ) : type( Value_symbol ), number( sym )
			{
			}
		
			Value( symbol_id sym, int ) : type( V_decl ), number( sym )
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
		
			Value( const proc_info& proc )
			:
				type( Value_function ),
				function( &proc )
			{
			}
		
			Value( const Value& a, const Value& b );
		
			Value( value_type vt, const Value& a, op_type op, const Value& b );
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
	value_type get_type( const Value& v )
	{
		return v.type;
	}
	
	inline
	bool get_bool( const Value& v )
	{
		return ! v.number.is_zero();
	}
	
	inline
	plus::integer& get_int( Value& v )
	{
		return v.number;
	}
	
	inline
	const plus::integer& get_int( const Value& v )
	{
		return v.number;
	}
	
	inline
	const plus::string& get_str( const Value& v )
	{
		return v.string;
	}
	
	inline
	const proc_info& get_proc( const Value& v )
	{
		return *v.function;
	}
	
	inline
	Expr* get_expr( const Value& v )
	{
		return v.expr.get();
	}
	
	inline
	bool is_undefined( const Value& v )
	{
		return get_type( v ) == Value_undefined;
	}
	
	inline
	bool is_empty( const Value& v )
	{
		return get_type( v ) == Value_empty_list;
	}
	
	inline
	bool is_symbol( const Value& v )
	{
		return get_type( v ) == Value_symbol  ||  get_type( v ) == V_decl;
	}
	
	inline
	bool is_function( const Value& v )
	{
		if ( get_type( v ) == Value_function )
		{
			return true;
		}
		
		if ( Expr* expr = v.expr.get() )
		{
			if ( expr->op == Op_bind_args )
			{
				return true;
			}
		}
		
		return false;
	}
	
	Value bind_args( const Value& f, const Value& arguments );
	
	unsigned long area( const Expr& expr );
	
	inline
	unsigned long area( const expr_box& box )
	{
		if ( Expr* expr = box.get() )
		{
			return sizeof (expr_box) + area( *expr );
		}
		
		return sizeof (expr_box);
	}
	
	inline
	unsigned long area( const Value& v )
	{
		return + sizeof (Value)
		       - sizeof v.string
		       - sizeof v.number
		       - sizeof v.expr
		       + area( v.string )
		       + area( v.number )
		       + area( v.expr   );
	}
	
}

#endif
