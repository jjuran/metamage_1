/*
	byteclass.hh
	------------
*/

#ifndef VLIB_TYPES_BYTECLASS_HH
#define VLIB_TYPES_BYTECLASS_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch byteclass_dispatch;
	
	class ByteClass : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &byteclass_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			ByteClass();
			ByteClass( unsigned char c );
			ByteClass( unsigned char a, unsigned char b );
			
			bool get( unsigned char i ) const;
			void set( unsigned char i );
	};
	
	ByteClass operator|( const ByteClass& a, const ByteClass& b );
	
	extern const type_info byteclass_vtype;
	
}

#endif
