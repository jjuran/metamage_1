/*
	box.hh
	------
*/

#ifndef VXO_BOX_HH
#define VXO_BOX_HH

// Standard C
#include <stddef.h>
#include <stdint.h>

// vxo
#include "vxo/datum_alloc.hh"


namespace vxo
{
	
	typedef datum_allocation  u_string;
	
	typedef const void* anyptr_t;
	
	enum
	{
		box_size = sizeof (u_string),
		
		inline_char_size = box_size - 1,
		inline_int_bytes = box_size - 2,
		ternary_byte_loc = box_size - 3,
	};
	
	enum box_type
	{
		Box_shared  = -2,  // 1111 1110
		Box_pointer = -1,  // 1111 1111
		
		// 0000 0000 - 000? 1111
		Box_min_inline_string = 0,
		Box_max_inline_string = inline_char_size,  // 15 or 31
		
		Box_inline_uint8  =  96,  // 0110 0000
		Box_inline_uint16 =  97,  // 0110 0001
		Box_inline_uint32 =  98,  // 0110 0010
		Box_inline_uint64 =  99,  // 0110 0011
		Box_inline_int8   = 100,  // 0110 0100
		Box_inline_int16  = 101,  // 0110 0101
		Box_inline_int32  = 102,  // 0110 0110
		Box_inline_int64  = 103,  // 0110 0111
		
		Box_vector_anyptr = 105 + sizeof (anyptr_t) / 4,  // 0110 101x
		
		Box_inline_float  = 110,  // 0110 1110
		Box_inline_double = 111,  // 0110 1111
		
		Box_container = 112,  // 0111 0000
		Box_array     = 112,  // 0111 0000
		Box_set       = 113,  // 0111 0001
		Box_map       = 115,  // 0111 0011
		
		Box_boolean   = 120,  // 0111 1000
		
		Box_symbolic  = 124,  // 0111 1100
		
		Box_undefined = 127,  // 0111 1111
		
		Box_error = -1,
		Box_string = 0,
	};
	
	typedef float  float_t;
	typedef double double_t;
	
	union vxo_u
	{
		u_string  str;
		char      chr[ box_size ];              // 16 or 32
	};
	
	class Box
	{
		protected:
			vxo_u u;
			
			void set_control_byte( char c )  { u.chr[ inline_char_size ] = c; }
			void set_subtype_byte( char c )  { u.chr[ inline_int_bytes ] = c; }
			void set_ternary_byte( char c )  { u.chr[ ternary_byte_loc ] = c; }
			
			void unshare()
			{
				if ( control_byte() == Box_shared )
				{
					unshare_extent();
				}
			}
		
		private:
			void unshare_extent();
			void destroy_extent();
			
			void destroy()
			{
				if ( has_extent() )
				{
					destroy_extent();
				}
			}
		
		public:
			~Box()  { destroy(); }
			
			Box()  { set_control_byte( Box_undefined ); }
			
			Box( const Box& that );
			
			Box& operator=( const Box& that );
			
			void swap( Box& b );
			
			int8_t control_byte() const  { return u.chr[ inline_char_size ]; }
			int8_t subtype_byte() const  { return u.chr[ inline_int_bytes ]; }
			int8_t ternary_byte() const  { return u.chr[ ternary_byte_loc ]; }
			
			int8_t datatype_byte() const
			{
				return u.chr[ inline_char_size - has_pointer() ];
			}
			
			bool has_pointer() const  { return control_byte() < 0; }
			bool has_extent()  const  { return control_byte() < Box_pointer; }
			
			bool undefined() const  { return control_byte() == Box_undefined; }
			bool   defined() const  { return control_byte() != Box_undefined; }
			
			// Boolean context conversion
			operator const volatile Box*() const
			{
				return defined()  &&  datatype_byte() >= 0 ? this : NULL;
			}
			
			template < class Derived >
			Derived* is()
			{
				return Derived::test( *this ) ? static_cast< Derived* >( this )
				                              : NULL;
			}
			
			template < class Derived >
			const Derived* is() const
			{
				return const_cast< Box* >( this )->is< Derived >();
			}
	};
	
	inline
	void swap( Box& a, Box& b )
	{
		a.swap( b );
	}
	
}

#endif
