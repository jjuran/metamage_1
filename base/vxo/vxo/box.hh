/*
	box.hh
	------
*/

#ifndef VXO_BOX_HH
#define VXO_BOX_HH

// Standard C
#include <stddef.h>
#include <stdint.h>

// plus
#include "plus/datum_alloc.hh"


namespace vxo
{
	
	typedef plus::datum_allocation  u_string;
	
	enum
	{
		box_size = sizeof (u_string),
		
		inline_char_size = box_size - 1,
		inline_int_bytes = box_size - 2,
	};
	
	enum box_type
	{
		Box_shared  = -2,  // 1111 1110
		Box_pointer = -1,  // 1111 1111
		
		// 0000 0000 - 000? 1111
		Box_min_inline_string = 0,
		Box_max_inline_string = inline_char_size,  // 15 or 31
		
		Box_container = 112,  // 0111 0000
		Box_array     = 112,  // 0111 0000
		Box_set       = 113,  // 0111 0001
		Box_map       = 115,  // 0111 0011
		
		Box_undefined = 127,  // 0111 1111
		
		Box_error = -1,
		Box_string = 0,
	};
	
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
		
		private:
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
