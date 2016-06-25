/*
	value.hh
	--------
*/

#ifndef VLIB_VALUE_HH
#define VLIB_VALUE_HH

// plus
#include "plus/integer_fwd.hh"
#include "plus/string.hh"

// vlib
#include "vlib/op_type.hh"
#include "vlib/source.hh"
#include "vlib/vbox.hh"


namespace vlib
{
	
	enum Bool
	{
		False,
		True,
	};
	
	enum symbol_type
	{
		Symbol_const,
		Symbol_var,
	};
	
	struct proc_info;
	struct type_info;
	struct Expr;
	class Symbol;
	
	enum value_type
	{
		Value_nothing,
		Value_dummy_operand,
		Value_undefined,
		Value_expired,
		Value_empty_list,
		Value_symbol,
		Value_symbol_descriptor,
		Value_base_type,
		Value_boolean,
		Value_byte,
		Value_number,
		Value_string,
		Value_data,
		Value_function,
		Value_pair,
		
		V_dummy = Value_dummy_operand,
		V_desc  = Value_symbol_descriptor,
		V_bool  = Value_boolean,
		V_int   = Value_number,
		V_str   = Value_string,
		V_data  = Value_data,
		V_proc  = Value_function,
	};
	
	struct data_tag {};
	
	class Value
	{
		private:
			vbox  its_box;
			
			friend void pair_destructor( void* pointer );
		
		protected:
			Value( const vu_ibox& ix, value_type type )
			:
				its_box( ix, type )
			{
			}
			
			Value( const vu_string& sx, value_type type )
			:
				its_box( sx, type )
			{
			}
		
		public:
			enum symdesc
			{
				symdesc_min = 0,
				symdesc_max = 0xFFFFFFFFu,
			};
			
			Value( value_type type = value_type() ) : its_box( type )
			{
			}
			
			Value( symdesc desc ) : its_box( uint32_t( desc), V_desc )
			{
			}
			
			Value( Bool b ) : its_box( bool( b ), Value_boolean )
			{
			}
			
		#define IBOX( i )  plus::ibox( i ).move()
			
			Value( bool b ) : its_box( b, Value_boolean )
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
			
			Value( const plus::string& s, data_tag )
			:
				its_box( (const vu_string&) s, V_data )
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
			
			Value( const type_info& type )
			:
				its_box( &type, Value_base_type )
			{
			}
			
			Value( symbol_type symtype, const plus::string& name );
			
			Value( const Value& a, const Value& b );
			
			Value( op_type op, const Value& v );
			
			Value( const Value&        a,
			       op_type             op,
			       const Value&        b,
			       const source_spec&  s = source_spec() );
			
			value_type type() const
			{
				return value_type( its_box.semantics() );
			}
			
			bool boolean() const
			{
				return its_box.boolean();
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
			
			const type_info& typeinfo() const
			{
				return *(const type_info*) its_box.pointer();
			}
			
			symdesc desc() const
			{
				if ( type() == V_desc )
				{
					return symdesc( its_box.u32() );
				}
				
				return symdesc( -1 );
			}
			
			Symbol* sym() const
			{
				if ( type() == Value_symbol )
				{
					return (Symbol*) its_box.pointer();
				}
				
				return decl_sym();
			}
			
			Symbol* decl_sym() const;
			
			operator Symbol*() const
			{
				return sym();
			}
			
			Expr* expr() const
			{
				if ( its_box.semantics() == Value_pair )
				{
					return (Expr*) its_box.pointer();
				}
				
				return 0;  // NULL
			}
			
			Expr* listexpr() const;
			
			void swap( Value& that );
			
			friend unsigned long area( const Value& v );
	};
	
	inline
	void swap( Value& a, Value& b )
	{
		a.swap( b );
	}
	
	extern const Value nothing;
	extern const Value empty_list;
	
	typedef Value symbol_id;
	
	struct Expr
	{
		const op_type  op;
		Value          left;
		Value          right;
		
		const source_spec source;
		
		Expr( const Value&        a,
		      op_type             op,
		      const Value&        b,
		      const source_spec&  s = source_spec() );
	};
	
	inline
	Symbol* Value::decl_sym() const
	{
		if ( Expr* ex = expr() )
		{
			if ( ex->op == Op_var  ||  ex->op == Op_const )
			{
				return ex->right.sym();
			}
		}
		
		return 0;  // NULL
	}
	
	template < class Type >
	static
	Value assign_to( const Value& v )
	{
		if ( Type::test( v ) )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	inline
	bool get_bool( const Value& v )
	{
		return v.boolean();
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
		return v.type() == Value_undefined;
	}
	
	inline
	bool is_empty( const Value& v )
	{
		return v.type() == Value_empty_list;
	}
	
	inline
	bool is_single( const Value& v )
	{
		return ! is_empty( v )  &&  ! v.listexpr();
	}
	
	inline
	bool is_type_annotation( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			return expr->op == Op_denote;
		}
		
		return false;
	}
	
	inline
	bool is_symbol_declarator( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			return expr->op == Op_var  ||  expr->op == Op_const;
		}
		
		return false;
	}
	
	inline
	bool is_symbol( const Value& v )
	{
		return v.type() == Value_symbol  ||  is_symbol_declarator( v );
	}
	
	inline
	bool is_type( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_subscript )
			{
				if ( is_type( expr->left ) )
				{
					// subscript type
					return is_empty( expr->right );
				}
			}
			
			if ( expr->op == Op_mapping )
			{
				return is_type( expr->left )  &&  is_type( expr->right );
			}
			
			if ( expr->op == Op_intersection  ||  expr->op == Op_union )
			{
				return is_type( expr->left )  &&  is_type( expr->right );
			}
		}
		
		return v.type() == Value_base_type  ||  is_empty( v );
	}
	
	inline
	bool is_function( const Value& v )
	{
		if ( v.type() == Value_function )
		{
			return true;
		}
		
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_bind_args  ||  expr->op == Op_invocation )
			{
				return true;
			}
			
			if ( expr->op == Op_lambda )
			{
				return true;
			}
		}
		
		return false;
	}
	
	inline
	bool is_functional( const Value& v )
	{
		return is_function( v )  ||  v.type() == Value_base_type;
	}
	
	inline
	Value make_data( const plus::string& s )
	{
		return Value( s, data_tag() );
	}
	
	inline
	Value make_array( const Value& list )
	{
		return Value( Op_array, list );
	}
	
	Value bind_args( const Value& f, const Value& arguments );
	
	unsigned long area( const Value& v );
	
	const Value& first( const Value& list );
	const Value& rest ( const Value& list );
	
}

#endif
