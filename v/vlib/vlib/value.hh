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
		Bool_false,
		Bool_true,
	};
	
	enum symbol_type
	{
		Symbol_const,
		Symbol_var,
	};
	
	struct dispatch;
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
		Value_empty_array,
		Value_symbol,
		Value_symbol_descriptor,
		Value_base_type,
		Value_boolean,
		Value_byte,
		Value_mb32,
		Value_number,
		Value_string,
		Value_packed,
		Value_function,
		Value_pair,
		
		V_dummy = Value_dummy_operand,
		V_desc  = Value_symbol_descriptor,
		V_bool  = Value_boolean,
		V_int   = Value_number,
		V_str   = Value_string,
		V_pack  = Value_packed,
		V_proc  = Value_function,
	};
	
	struct mutable_list_overrun {};
	
	class Value
	{
		private:
			const dispatch*  its_dispatch;
			vbox             its_box;
			
			friend void pair_destructor( void* pointer );
		
		protected:
			Value( Bool b, const dispatch* d )
			:
				its_dispatch( d ),
				its_box( bool( b ), Value_boolean )
			{
			}
			
			Value( const vu_ibox& ix, value_type type, const dispatch* d )
			:
				its_box( ix, type )
			{
				its_dispatch = d;
			}
			
			Value( const vu_string& sx, value_type type, const dispatch* d )
			:
				its_box( sx, type )
			{
				its_dispatch = d;
			}
			
			Value( const proc_info& proc, const dispatch* d )
			:
				its_box( &proc, V_proc )
			{
				its_dispatch = d;
			}
			
			Value( const type_info& type, const dispatch* d )
			:
				its_box( &type, Value_base_type )
			{
				its_dispatch = d;
			}
			
			Value( const Value&        a,
			       op_type             op,
			       const Value&        b,
			       const dispatch*     d );
			
			Value( long n, destructor dtor, value_type t, const dispatch* d );
			
			const void* pointer() const
			{
				return its_box.pointer();
			}
		
		public:
			enum symdesc
			{
				symdesc_min = 0,
				symdesc_max = 0xFFFFFFFFu,
			};
			
			template < class Type >
			Type to() const
			{
				Value const& coerced = Type::coerce( *this );
				return static_cast< Type const& >( coerced );
			}
			
			Value( value_type type = value_type() ) : its_box( type )
			{
				its_dispatch = 0;  // NULL
			}
			
			Value( symdesc desc ) : its_box( uint32_t( desc), V_desc )
			{
				its_dispatch = 0;  // NULL
			}
			
			Value( const Value& a, const Value& b );
			
			Value( op_type op, const Value& v );
			
			Value( const Value&        a,
			       op_type             op,
			       const Value&        b,
			       const source_spec&  s = source_spec() );
			
			const dispatch* dispatch_methods() const
			{
				return its_dispatch;
			}
			
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
			
			template < class T >
			const T& dereference() const
			{
				return *static_cast< const T* >( its_box.pointer() );
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
			
			const Value& secret() const;
			
			Value& unshare();
			
			friend unsigned long area( const Value& v );
	};
	
	inline
	void swap( Value& a, Value& b )
	{
		a.swap( b );
	}
	
	extern const Value nothing;
	extern const Value dummy_operand;
	extern const Value undefined;
	extern const Value empty_list;
	extern const Value empty_array;
	
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
			if ( declares_symbols( ex->op ) )
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
	bool is_undefined( const Value& v )
	{
		return v.type() == Value_undefined;
	}
	
	inline
	bool is_empty_list( const Value& v )
	{
		return v.type() == Value_empty_list;
	}
	
	inline
	bool is_empty_array( const Value& v )
	{
		return v.type() == Value_empty_array;
	}
	
	inline
	bool is_array( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			return expr->op == Op_array;
		}
		
		return is_empty_array( v );
	}
	
	inline
	bool is_single( const Value& v )
	{
		return ! is_empty_list( v )  &&  ! v.listexpr();
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
			return declares_symbols( expr->op );
		}
		
		return false;
	}
	
	inline
	bool is_symbol( const Value& v )
	{
		return v.type() == Value_symbol  ||  is_symbol_declarator( v );
	}
	
	bool is_type( const Value& v );
	
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
	Value make_array( const Value& list )
	{
		return is_empty_list( list ) ? empty_array
		                             : Value( Op_array, list );
	}
	
	Value bind_args( const Value& f, const Value& arguments );
	
	unsigned long area( const Value& v );
	
	const Value& first( const Value& list );
	const Value& rest ( const Value& list );
	
	Value& first_mutable( Value& list );
	Value& rest_mutable ( Value& list );
	
}

#endif
