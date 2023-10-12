/*
	ibox.hh
	-------
*/

#ifndef BIGNUM_IBOX_HH
#define BIGNUM_IBOX_HH

// math
#include "math/integer_types.hh"

/*
	ibox:  Variable-width integer storage unit
	------------------------------------------
	
	An ibox is a container for storaging the bits of a signed integer of
	arbitrary width, without allocating memory for integers which fit into a
	machine register.
*/


namespace bignum
{
	
	enum
	{
		Sign_negative = -1,
		Sign_null     =  0,
		Sign_positive =  1,
	};
	
	/*
		The constructors for unsigned long and long assume that limb_t is
		at least the size of unsigned long.  This will fail if limb_t is
		a 32-bit unsigned int while long is 64 bits, for example.
		
		It's possible to make the code correct for this scenario, but I
		don't see a use case.
	*/
	
	struct ibox_structure
	{
		typedef math::integer::limb_t   int_t;
		typedef math::integer::limb_t*  ptr_t;
	
		typedef math::integer::size_t  size_t;
		typedef short                  sign_t;
		
		union
		{
			int_t  integer;
			ptr_t  pointer;
		};
		
		size_t  size;  // 0 if small, otherwise the number of limbs
		sign_t  sign;  // 0 if zero, 1 for positive, -1 for negative
	};
	
	typedef ibox_structure ibox_movable;
	
	class ibox
	{
		public:
			typedef math::integer::limb_t   int_t;
			typedef math::integer::limb_t*  ptr_t;
		
			typedef math::integer::size_t  size_t;
			typedef short                  sign_t;
			
			struct limb_count_overflow {};
		
		private:
			ibox_structure  its;
		
		private:
			bool has_extent() const  { return its.size != 0; }
			
			void destroy_extent();
			
			void destroy()
			{
				if ( has_extent() )
				{
					destroy_extent();
				}
			}
			
			void unshare();
			
			void construct( unsigned long i )
			{
				its.integer = i;
				its.size    = 0;
				its.sign    = i > 0;
			}
			
			void construct( long i )
			{
				its.size = 0;
				its.sign = (i > 0) - (i < 0);
				
				// 0x8000 0000 [0000 0000]
				const long nadir = 1UL << (sizeof (long) * 8 - 1);
				
				if ( i < 0  &&  i != nadir )
				{
					i = -i;
				}
				
				its.integer = i;
			}
			
			void construct( long long i );
			
			void construct( int_t const* data, size_t n, sign_t sign );
			
			int_t extent_top() const;
			int_t extent_bottom() const;
			
			unsigned long long long_bottom() const;
			
			void extend( size_t n );
			
			void shrink_to_fit();
		
		public:
			~ibox()
			{
				destroy();
			}
			
			ibox()
			{
				its.integer = 0;
				its.size    = 0;
				its.sign    = 0;
			}
			
			ibox( int_t const* data, size_t n, sign_t sign = Sign_positive )
			{
				if ( n > 1 )
				{
					construct( data, n, sign );
				}
				else if ( n > 0 )
				{
					construct( *data );
				}
				else
				{
					construct( 0ul );
				}
			}
			
			ibox( unsigned long i )
			{
				construct( i );
			}
			
			ibox( long i )
			{
				construct( i );
			}
			
			ibox( unsigned long long i )
			{
				if ( i == (unsigned long) i )
				{
					construct( (unsigned long) i );
				}
				else
				{
					const int n_limbs = sizeof (long long) / sizeof (int_t);
					
					construct( (int_t const*) &i, n_limbs, Sign_positive );
				}
			}
			
			ibox( long long i )
			{
				if ( i == (long) i )
				{
					construct( (long) i );
				}
				else if ( i == (long long) (unsigned long) i )
				{
					construct( (unsigned long) i );
				}
				else
				{
					construct( i );
				}
			}
			
			ibox( const ibox& that );
			
			ibox& operator=( const ibox& that );
			
			void swap( ibox& i );
			
			ibox_movable& move()  { return (ibox_movable&) *this; }
			
			size_t size() const
			{
				return has_extent() ? its.size : its.integer != 0;
			}
			
			int_t const* data() const
			{
				return has_extent() ? its.pointer : &its.integer;
			}
			
			int_t top() const
			{
				return has_extent() ? extent_top() : its.integer;
			}
			
			int_t bottom() const
			{
				return has_extent() ? extent_bottom() : its.integer;
			}
			
			bool odd() const { return bottom() & 1; }
			bool even() const { return ! odd(); }
			
			sign_t sign() const  { return its.sign; }
			
			template < class Int >  Int  clipped_to() const;
			template < class Int >  bool demotes_to() const;
			
			void invert()
			{
				its.sign = -its.sign;
			}
			
			void add( const ibox& y );
			
			void subtract_lesser( const ibox& y );
			void subtract       ( const ibox& y );
			
			void multiply_by( const ibox& y );
			
			void halve();
			
			unsigned long area() const;
	};
	
	template < class Int >
	inline
	Int ibox::clipped_to() const
	{
		const bool larger_type = sizeof (Int) > sizeof (int_t);
		
		if ( larger_type )
		{
			unsigned long long x = long_bottom();
			
			if ( sign() < 0 )
			{
				x = -x;
			}
			
			return Int( x );
		}
		else
		{
			int_t x = bottom();
			
			if ( sign() < 0 )
			{
				x = -x;
			}
			
			return Int( x );
		}
	}
	
	template < class Int >
	inline
	bool ibox::demotes_to() const
	{
		const bool smaller_type = sizeof (Int) < sizeof (int_t);
		const bool larger_type  = sizeof (Int) > sizeof (int_t);
		const int limbs_per_int = sizeof (Int) / sizeof (int_t);
		
		/*
			First cut:  Reject an integer with more limbs than the target
			type has.
		*/
		
		if ( larger_type  &&  its.size > limbs_per_int )
		{
			return false;
		}
		
		if ( ! larger_type  &&  has_extent() )
		{
			return false;
		}
		
		/*
			Second cut:  Reject a negative integer if the target type is
			unsigned.
		*/
		
		const Int signed_min = Int( 1ULL << (sizeof (Int) * 8 - 1) );
		const Int signed_max = Int( ~signed_min );
		
		const bool is_signed = Int( -1 ) < 0;
		
		if ( ! is_signed  &&  sign() < 0 )
		{
			return false;
		}
		
		/*
			Third cut:  Reject an integer that overflows a large signed type.
			Accept a large result that hasn't been rejected.
		*/
		
		if ( larger_type )
		{
			// Multiplying by the condition is a no-op to shut up a warning.
			if ( ! is_signed  ||  its.size * larger_type < limbs_per_int )
			{
				return true;
			}
			
			// FIXME:  This assumes that Int is the same size as long long.
			unsigned long long result = *(unsigned long long*) data();
			
			return result - (sign() < 0) <= (unsigned long long) signed_max;
		}
		
		/*
			Fourth cut:  Reject an integer that overflows the target type.
		*/
		
		if ( is_signed )
		{
			// Multiplying by the condition is a no-op to shut up a warning.
			return its.integer - (sign() < 0) <= signed_max * ! larger_type;
		}
		
		if ( smaller_type )
		{
			// Multiplying by the condition is a no-op to shut up a warning.
			return its.integer <= (int_t) Int( -1 ) * smaller_type;
		}
		
		return true;
	}
	
	math::integer::cmp_t abs_compare( const ibox& a, const ibox& b );
	math::integer::cmp_t     compare( const ibox& a, const ibox& b );
	
	inline
	void swap( ibox& a, ibox& b )
	{
		a.swap( b );
	}
	
	inline
	unsigned long area( const ibox& box )
	{
		return box.area();
	}
	
}

#endif
