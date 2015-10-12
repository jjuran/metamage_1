/*
	value.hh
	--------
*/

#ifndef VLIB_VALUE_HH
#define VLIB_VALUE_HH

// plus
#include "plus/integer.hh"
#include "plus/string.hh"

// vlib
#include "vlib/op_type.hh"
#include "vlib/symbol_id.hh"
#include "vlib/vbox.hh"


namespace vlib
{
	
	struct proc_info;
	struct Expr;
	
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
			vbox  its_box;
			
			friend void pair_destructor( void* pointer );
		
		public:
			Value( value_type type = value_type() ) : its_box( type )
			{
			}
			
		#define IBOX( i )  plus::ibox( i ).move()
			
			Value( symbol_id sym ) : its_box( IBOX( (long) sym ), Value_symbol )
			{
			}
			
			Value( symbol_id sym, int ) : its_box( IBOX( (long) sym ), V_decl )
			{
			}
			
			Value( bool b ) : its_box( IBOX( (long) b ), Value_boolean )
			{
			}
			
			Value( unsigned i ) : its_box( IBOX( (unsigned long) i ), V_int ) {}
			Value(      int i ) : its_box( IBOX( (long)          i ), V_int ) {}
			
			Value( unsigned long i ) : its_box( IBOX( i ), V_int ) {}
			Value(          long i ) : its_box( IBOX( i ), V_int ) {}
			
			Value( unsigned long long i ) : its_box( IBOX( i ), V_int ) {}
			Value(          long long i ) : its_box( IBOX( i ), V_int ) {}
			
		#undef IBOX
			
			Value( const plus::integer& i )
			:
				its_box( (const vu_ibox&) i, V_int )
			{
			}
			
			Value( const plus::string& s )
			:
				its_box( (const vu_string&) s, V_str )
			{
			}
			
			Value( const char* s )
			:
				its_box( (const vu_string&) plus::string( s ).move(), V_str )
			{
			}
			
			Value( const proc_info& proc )
			:
				its_box( &proc, V_proc )
			{
			}
			
			Value( const Value& a, const Value& b );
			
			Value( value_type vt, const Value& a, op_type op, const Value& b );
			
			value_type type() const
			{
				return value_type( its_box.semantics() );
			}
			
			plus::integer& number()
			{
				return *(plus::integer*) &its_box;
			}
			
			const plus::integer& number() const
			{
				return *(const plus::integer*) &its_box;
			}
			
			const plus::string& string() const
			{
				return *(const plus::string*) &its_box;
			}
			
			const proc_info& proc() const
			{
				return *(const proc_info*) its_box.pointer();
			}
			
			Expr* expr() const
			{
				if ( its_box.semantics() == Value_pair )
				{
					return (Expr*) its_box.pointer();
				}
				
				return 0;  // NULL
			}
			
			void swap( Value& that );
			
			friend unsigned long area( const Value& v );
	};
	
	inline
	void swap( Value& a, Value& b )
	{
		a.swap( b );
	}
	
	extern const Value empty_list;
	
	struct Expr
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
	
	unsigned long area( const Value& v );
	
}

#endif
