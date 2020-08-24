/*
	value.hh
	--------
*/

#ifndef VLIB_VALUE_HH
#define VLIB_VALUE_HH

// plus
#include "plus/string.hh"

// bignum
#include "bignum/integer_fwd.hh"

// vlib
#include "vlib/op_type.hh"
#include "vlib/source.hh"
#include "vlib/vbox.hh"


namespace vlib
{
	
	enum
	{
		Type_pure = 1,
	};
	
	enum symbol_type
	{
		Symbol_const,
		Symbol_var,
	};
	
	struct dispatch;
	struct type_info;
	struct Expr;
	class Symbol;
	
	enum value_type
	{
		Value_NIL,
		Value_dummy_operand,
		Value_undefined,
		Value_expired,
		Value_nothing,
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
		Value_pair,
		Value_other,
		
		V_dummy = Value_dummy_operand,
		V_desc  = Value_symbol_descriptor,
		V_bool  = Value_boolean,
		V_int   = Value_number,
		V_str   = Value_string,
		V_pack  = Value_packed,
	};
	
	enum value_flags
	{
		Flag_cycle_free,
		Flag_evaluated,
	};
	
	struct invalid_cast {};
	
	class Value
	{
		private:
			const dispatch*  its_dispatch;
			vbox             its_box;
			
			typedef const struct opaque* unspecified_boolean;
			
			friend void pair_destructor( void* pointer );
			
			bool flag_bit( int b ) const  { return its_box.flags() & (1 << b); }
			
			void set_flag( int bit )
			{
				its_box.set_flags( its_box.flags() | (1 << bit) );
			}
			
			void clear_flag( int bit )
			{
				its_box.set_flags( its_box.flags() & ~(1 << bit) );
			}
		
		protected:
			Value( value_type type, const dispatch* d )
			:
				its_dispatch( d ),
				its_box( type,
				         !! 1 << Flag_cycle_free |
				         !! d << Flag_evaluated )
			{
			}
			
			Value( const vu_ibox& ix, value_type type, const dispatch* d )
			:
				its_box( ix,
				         type,
				         1 << Flag_cycle_free |
				         1 << Flag_evaluated )
			{
				its_dispatch = d;
			}
			
			Value( const vu_string& sx, value_type type, const dispatch* d )
			:
				its_box( sx, type, 1 << Flag_cycle_free )
			{
				its_dispatch = d;
			}
			
			Value( const type_info& type, const dispatch* d )
			:
				its_box( &type,
				         Value_base_type,
				         !! 1 << Flag_cycle_free |
				         !! d << Flag_evaluated )
			{
				its_dispatch = d;
			}
			
			Value( const Value&        a,
			       op_type             op,
			       const Value&        b,
			       const dispatch*     d );
			
			Value( long n, destructor dtor, value_type t, const dispatch* d );
			
			void set_cycle_free()    { set_flag  ( Flag_cycle_free ); }
			void clear_cycle_free()  { clear_flag( Flag_cycle_free ); }
			
			void set_evaluated()     { set_flag  ( Flag_evaluated ); }
			
			const void* pointer() const
			{
				return its_box.pointer();
			}
			
			template < class T >
			T const& pod_cast() const
			{
				return *reinterpret_cast< T const* >( &its_box );
			}
			
			template < class T >
			T& pod_cast()
			{
				return *reinterpret_cast< T* >( &its_box );
			}
		
		public:
			void replace_dispatch_methods( const dispatch* d )
			{
				/*
					Do not call this unless you know what you're doing.
				*/
				
				its_dispatch = d;
			}
			
			bool has_extent() const  { return its_box.has_extent(); }
			
			bool is_cycle_free() const  { return flag_bit( Flag_cycle_free ); }
			bool is_evaluated()  const  { return flag_bit( Flag_evaluated  ); }
			
			void spoil( Value& new_container ) const
			{
				if ( ! is_cycle_free() )
				{
					new_container.clear_cycle_free();
				}
			}
			
			template < class Type >
			Type const* is() const
			{
				if ( Type::test( *this ) )
				{
					return static_cast< Type const* >( this );
				}
				
				return 0;  // NULL
			}
			
			template < class Type >
			Type const& as() const
			{
				if ( Type const* that = is< Type >() )
				{
					return *that;
				}
				
				throw invalid_cast();
			}
			
			template < class Type >
			Type to() const
			{
				Value const& coerced = Type::coerce( *this );
				return static_cast< Type const& >( coerced );
			}
			
			Value( value_type type = value_type() )
			:
				its_box( type,
				         1 << Flag_cycle_free |
				         1 << Flag_evaluated )
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
			
			operator unspecified_boolean() const
			{
				return unspecified_boolean( type() );
			}
			
			bignum::integer& number()
			{
				return *(bignum::integer*) &its_box;
			}
			
			const bignum::integer& number() const
			{
				return *(const bignum::integer*) &its_box;
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
			
			const type_info& typeinfo() const
			{
				return *(const type_info*) its_box.pointer();
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
	
	extern const Value NIL;
	extern const Value nothing;
	extern const Value dummy_operand;
	extern const Value undefined;
	extern const Value empty_list;
	extern const Value empty_array;
	
	typedef Value symbol_id;
	
	struct Expr
	{
		op_type  op;
		Value    left;
		Value    right;
		
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
	Value assign_to( const Value& v )
	{
		if ( Type::test( v ) )
		{
			return v;
		}
		
		return Value_NIL;
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
	
	inline
	bool is_block( const Value& v )
	{
		Expr* expr = v.expr();
		
		return expr  &&  expr->op == Op_invocation;
	}
	
	unsigned long area( const Value& v );
	
	const Value& first( const Value& list );
	const Value& rest ( const Value& list );
	
}

#endif
