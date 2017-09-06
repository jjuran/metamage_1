/*
	integer.hh
	----------
*/

#ifndef VLIB_TYPES_INTEGER_HH
#define VLIB_TYPES_INTEGER_HH

// iota
#include "iota/integer_cast.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch integer_dispatch;
	
	class Integer : public Value
	{
		typedef bignum::integer integer;
		
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_number;
			}
			
			static Value coerce( const Value& v );
			
		#define V_INT  V_int, &integer_dispatch
		#define IBOX( i )  bignum::ibox( i ).move()
			
			Integer() : Value( IBOX( 0ul ), V_INT ) {}
			
			Integer( unsigned i ) : Value( IBOX( (unsigned long) i ), V_INT ) {}
			Integer(      int i ) : Value( IBOX( (long)          i ), V_INT ) {}
			
			Integer( unsigned long i ) : Value( IBOX( i ), V_INT ) {}
			Integer(          long i ) : Value( IBOX( i ), V_INT ) {}
			
			Integer( unsigned long long i ) : Value( IBOX( i ), V_INT ) {}
			Integer(          long long i ) : Value( IBOX( i ), V_INT ) {}
			
		#undef IBOX
			
			Integer( const integer& i ) : Value( (const vu_ibox&) i, V_INT ) {}
			
		#undef V_INT
			
			const bignum::integer& get() const  { return number(); }
			
			operator const bignum::integer&() const  { return number(); }
	};
	
	extern const type_info integer_vtype;
	
	struct bad_cast_thrower
	{
		void operator()() const;
	};
	
	template < class Out >
	inline
	Out integer_cast( const Integer& i )
	{
		using bignum::integer_cast;
		
		return integer_cast< Out >( i.number(), bad_cast_thrower() );
	}
	
	template < class Out >
	inline
	Out integer_cast( const Value& v )
	{
		if ( ! Integer::test( v ) )
		{
			bad_cast_thrower()();
		}
		
		const Integer& i = static_cast< const Integer& >( v );
		
		return integer_cast< Out >( i );
	}
	
}

#endif
