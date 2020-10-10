/*
	number.hh
	---------
*/

#ifndef VXO_NUMBER_HH
#define VXO_NUMBER_HH

// vxo
#include "vxo/box.hh"


namespace vxo
{
	
	class Number : public Box
	{
		public:
			enum
			{
				box_type_base = 0x60,  // 0110 0000
				box_type_mask = 0xF0,  // 1111 0000
				box_size_mask = 0x03,  // 0000 0011, for both ints and FP
				box_sign_mask = 0x04,  // 0000 0100
			};
			
			static bool test( const Box& box )
			{
				return (box.control_byte() & box_type_mask) == box_type_base;
			}
			
			#define NUM_TYPE(Type)  \
			explicit Number( Type##_t x )  \
			{ *(Type##_t*) &u = x;  set_control_byte( Box_inline_##Type ); }
			
			NUM_TYPE(  int8  );
			NUM_TYPE( uint8  );
			NUM_TYPE(  int16 );
			NUM_TYPE( uint16 );
			NUM_TYPE(  int32 );
			NUM_TYPE( uint32 );
			NUM_TYPE(  int64 );
			NUM_TYPE( uint64 );
			NUM_TYPE( float  );
			NUM_TYPE( double );
			
			#undef NUM_TYPE
			
			unsigned sizeof_numeric_unit() const
			{
				return 1 << (datatype_byte() & box_size_mask);
			}
	};
	
	class Integer : public Number
	{
		public:
			enum
			{
				box_type_base = 0x60,  // 0110 0000
				box_type_mask = 0xF8,  // 1111 1000
			};
			
			static bool test( const Box& box )
			{
				return (box.control_byte() & box_type_mask) == box_type_base;
			}
			
			#define NUM_TYPE(Type)  \
			explicit Integer( Type##_t x ) : Number( x )  {}
			
			NUM_TYPE(  int8  );
			NUM_TYPE( uint8  );
			NUM_TYPE(  int16 );
			NUM_TYPE( uint16 );
			NUM_TYPE(  int32 );
			NUM_TYPE( uint32 );
			NUM_TYPE(  int64 );
			NUM_TYPE( uint64 );
			
			#undef NUM_TYPE
			
			bool is_signed() const  { return datatype_byte() & box_sign_mask; }
	};
	
	class FloatingPoint : public Number
	{
		public:
			enum
			{
				box_type_base = 0x6E,  // 0110 1110
				box_type_mask = 0xFE,  // 1111 1110
			};
			
			static bool test( const Box& box )
			{
				return (box.control_byte() & box_type_mask) == box_type_base;
			}
			
			#define NUM_TYPE(Type)  \
			explicit FloatingPoint( Type##_t x ) : Number( x )  {}
			
			NUM_TYPE( float  );
			NUM_TYPE( double );
			
			#undef NUM_TYPE
	};
	
}

#endif
