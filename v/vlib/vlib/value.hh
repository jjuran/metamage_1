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
	
	class Value
	{
		private:
			value_type     its_type;
			plus::integer  its_int;
			plus::string   its_str;
			proc_t         its_proc;
			expr_box       its_expr;
			
			friend class expr_box;
		
		public:
			Value( value_type type = value_type() ) : its_type( type )
			{
			}
			
			Value( symbol_id sym ) : its_type( Value_symbol ), its_int( sym )
			{
			}
			
			Value( symbol_id sym, int ) : its_type( V_decl ), its_int( sym )
			{
			}
			
			Value( bool b ) : its_type( Value_boolean ), its_int( b )
			{
			}
			
			Value( unsigned int i ) : its_type( V_int ), its_int( i ) {}
			Value(          int i ) : its_type( V_int ), its_int( i ) {}
			
			Value( unsigned long i ) : its_type( V_int ), its_int( i ) {}
			Value(          long i ) : its_type( V_int ), its_int( i ) {}
			
			Value( unsigned long long i ) : its_type( V_int ), its_int( i ) {}
			Value(          long long i ) : its_type( V_int ), its_int( i ) {}
			
			Value( const plus::integer& i ) : its_type( V_int ), its_int( i )
			{
			}
			
			Value( const plus::string& s ) : its_type( V_str ), its_str( s )
			{
			}
			
			Value( const char* s ) : its_type( V_str ), its_str( s )
			{
			}
			
			Value( const proc_info& proc )
			:
				its_type( Value_function ),
				its_proc( &proc )
			{
			}
			
			Value( const Value& a, const Value& b );
			
			Value( value_type vt, const Value& a, op_type op, const Value& b );
			
			value_type type() const  { return its_type; }
			
			plus::integer&       number()        { return its_int; }
			const plus::integer& number() const  { return its_int; }
			const plus::string&  string() const  { return its_str; }
			
			const proc_info& proc() const  { return *its_proc; }
			
			Expr* expr() const  { return its_expr.get(); }
			
			void swap( Value& that );
			
			friend unsigned long area( const Value& v );
	};
	
	inline
	void swap( Value& a, Value& b )
	{
		a.swap( b );
	}
	
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
		return v.type();
	}
	
	inline
	bool get_bool( const Value& v )
	{
		return ! v.number().is_zero();
	}
	
	inline
	plus::integer& get_int( Value& v )
	{
		return v.number();
	}
	
	inline
	const plus::integer& get_int( const Value& v )
	{
		return v.number();
	}
	
	inline
	const plus::string& get_str( const Value& v )
	{
		return v.string();
	}
	
	inline
	const proc_info& get_proc( const Value& v )
	{
		return v.proc();
	}
	
	inline
	Expr* get_expr( const Value& v )
	{
		return v.expr();
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
		
		if ( Expr* expr = v.expr() )
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
		       - sizeof v.its_str
		       - sizeof v.its_int
		       - sizeof v.its_expr
		       + area( v.its_str  )
		       + area( v.its_int  )
		       + area( v.its_expr );
	}
	
}

#endif
